#!/usr/bin/env python
import itertools
import logging
import optparse
import os
import struct
import subprocess
import sys
import tempfile

from collections import namedtuple

log = logging.getLogger(__name__)


def _run_command(cmd):
    """
    Run a command and return the stdout for convenience
    """
    log.debug("Spawning %s" % ' '.join(cmd))
    out = tempfile.NamedTemporaryFile()
    err = tempfile.NamedTemporaryFile()
    proc = subprocess.Popen(cmd, stdout = out, stderr = err)
    rcode = proc.wait()
    out.seek(0)
    err.seek(0)
    if rcode != 0:
        logging.error(out.read())
        logging.error(err.read())
        raise Exception("%s failed with exit code %d" % (cmd[0], rcode))
    output = out.read().strip()
    error = err.read().strip()
    if len(output):
        log.debug(output)
    if len(error):
        log.warn(error)
    return output


class BaseInstaller(object):
    """
    Generic logic for installation, relying on specific filesystem detailed
    implementation of operations to copy into image, get sector list, etc.
    """
    
    # This is where the stage2 will be loaded
    stage2_offset = 0x7e00

    def __init__(self, disk, loader_base):
        """
        Constructor
        """
        self.disk = disk
        self.stage1 = os.path.join(loader_base, 'stage1-%s.bin' % self.fs_suffix)
        self.stage2 = os.path.join(loader_base, 'stage2.bin')
        self.inner_stage2 = 'stage2.bin'

    def install(self):
        """
        Just install the system
        """
        try:
            self.prepare()
            self.install_stage2()
            self.install_stage1()
        finally:
            self.finalize()

    def install_stage2(self):
        """
        Copy the stage2 image into the disk image
        """
        log.info('Installing stage2 (%s)' % self.stage2)
        self.copy(self.stage2, self.inner_stage2)

    def _avoid_64k_boundary(self, sector_ranges):
        """
        Split the ranges, so there is no one running over a 64k boundary
        """
        blocks_per_barrier = 2**16 / 512
        prepared = list()

        for start, count in sector_ranges:
            # Fit first in the space between the offset and the first barrier
            available_blocks = (2**16 - self.stage2_offset) / 512
            subrange_start = start
            subrange_count = min(available_blocks, count)
            prepared.append((subrange_start, subrange_count))
            # Fit the rest in 64k blocks
            remaining = count - subrange_count
            while remaining > 0:
                subrange_start += subrange_count
                subrange_count = min(blocks_per_barrier, remaining)
                prepared.append((subrange_start, subrange_count))
                remaining -= subrange_count

        return prepared
        
    def install_stage1(self):
        """
        Install the stage1 into the disk image
        """
        log.info('Installing stage1')
        # Get the sectors where the stage2 is
        sector_ranges = self.get_sector_ranges(self.inner_stage2)

        # Prepare so there is no range over a 64k boundary
        sector_ranges = self._avoid_64k_boundary(sector_ranges)
        log.debug('Got sector list for %s' % self.inner_stage2)
        log.debug(', '.join([str(s) for s in sector_ranges]))

        # Generate a packed version
        int16_list = list(itertools.chain(*sector_ranges))
        format = '<' + 'H' * (len(int16_list) + 1)
        int16_list = int16_list + [len(sector_ranges)]
        payload = struct.pack(format, *int16_list)

        self.introduce_payload(payload)
        log.info('Introduced payload of %d bytes' % len(payload))
        log.debug(' '.join(["%02x" % ord(p) for p in payload]))
        
    def prepare(self):
        """
        Prepare stage (i.e. mount), optional
        """
        pass
    
    def finalize(self):
        """
        Termination stage (i.e. umount), optional
        """
        pass
    
    def copy(self, src, dst):
        """
        Copy src into the disk as dst
        """
        raise NotImplemented()
    
    def introduce_payload(self, payload):
        """
        The layout depends on the filesystem and specific loader, so delegate
        to the implementation
        """
        raise NotImplemented()


class FAT1xInstaller(BaseInstaller):
    """
    FAT1x implementation
    """
    
    BootSector = namedtuple('FATBootSector',
       ['jump', 'oem', 'bytes_per_sector', 'sectors_per_cluster', 'reserved_sectors',
        'number_of_fat', 'root_entries', 'total_sectors', 'media',
        'sectors_per_fat', 'sectors_per_track', 'heads_per_cyl',
        'hidden_sectors', 'total_sectors_big', 'drive_number', 'unused',
        'signature', 'serial', 'label', 'filesystem']
    )
    
    fs_suffix = 'fat1x'
    
    def copy(self, src, dst):
        """
        Copy the file src into the disk image as dst
        """
        _run_command(['mcopy', '-i', self.disk, '-o', src, '::/' + dst])
    
    def _get_fat_params(self):
        """
        Read the first bytes with the relevant FAT information
        """
        fd = open(self.disk, 'r')
        boot_sector = fd.read(62)
        unnamed_info = struct.unpack('<3s8sHBHBHHBHHHIIBBBI11s8s', boot_sector)
        named_info = FAT1xInstaller.BootSector._make(unnamed_info)
        log.debug(str(named_info))
        fd.close()
        return named_info
    
    def _get_clusters(self, fname):
        """
        Return the list of clusters containing fname
        """
        cmd_output = _run_command(['mshowfat', '-i', self.disk, '::/' + fname])
        open_pos = cmd_output.find('<')
        close_pos = cmd_output.find('>')
        cluster_list_str = cmd_output[open_pos + 1:close_pos]
        cluster_ranges = cluster_list_str.split(',')
        clusters = list()
        for cluster_range in cluster_ranges:
            start_str, end_str = cluster_range.split('-')
            start = int(start_str)
            nclusters = int(end_str) - start + 1
            clusters.append((start, nclusters))
        log.debug('Clusters for %s: %s' % (fname, ', '.join([str(i) for i in clusters])))
        return clusters
    
    def get_sector_ranges(self, fname):
        """
        Return the sector list of the file fname
        """
        fat = self._get_fat_params()
        clusters = self._get_clusters(fname)
        
        fat_size = fat.number_of_fat * fat.sectors_per_fat
        root_size = (fat.root_entries * 32) / fat.bytes_per_sector
        start_data_cluster = fat.reserved_sectors + fat_size + root_size
        
        sectors = map(
            lambda (cluster, count): ((start_data_cluster + cluster - 2) * fat.sectors_per_cluster, count * fat.sectors_per_cluster),
            clusters
        )
        return sectors
    
    def introduce_payload(self, payload):
        """
        Introduce the sector list at the end
        """       
        # Read first block from the disk
        disk_fd = open(self.disk, 'r+')
        disk_bs = disk_fd.read(512)
        
        # Introduce the code and payload at the end
        # Respect FS information and terminating 0xAA55!
        stage1_fd = open(self.stage1, 'r')
        stage1 = stage1_fd.read(512)
        stage1_fd.close()
        
        payload_size = len(payload)
        stage1 = disk_bs[0: 62] + stage1[62:510 - payload_size] + payload + stage1[510:512]
        
        # And write boot sector
        disk_fd.seek(0)
        disk_fd.write(stage1)
        disk_fd.close()


class FAT32Installer(FAT1xInstaller):
    """
    FAT32 implementation
    """
    
    BootSector = namedtuple('FATBootSector',
       ['jump', 'oem', 'bytes_per_sector', 'sectors_per_cluster', 'reserved_sectors',
        'number_of_fat', 'root_entries', 'total_sectors', 'media',
        'unused', 'sectors_per_track', 'heads_per_cylinder', 'hidden_sectors',
        'total_sectors_big', 'sectors_per_fat']
    )

    fs_suffix = 'fat32'
    
    def _get_fat_params(self):
        """
        Read the first bytes with the relevant FAT information
        """
        fd = open(self.disk, 'r')
        boot_sector = fd.read(40)
        unnamed_info = struct.unpack('<3s8sHBHBHHBHHHIII', boot_sector)
        named_info = FAT32Installer.BootSector._make(unnamed_info)
        log.debug(str(named_info))
        fd.close()
        return named_info

    def introduce_payload(self, payload):
        """
        Introduce the sector list at the end
        """
        # Read first block from the disk
        disk_fd = open(self.disk, 'r+')
        disk_bs = disk_fd.read(512)
        
        # Introduce the code and payload at the end
        # Respect FS information and terminating 0xAA55!
        stage1_fd = open(self.stage1, 'r')
        stage1 = stage1_fd.read(512)
        stage1_fd.close()
        
        payload_size = len(payload)
        stage1 = disk_bs[0: 90] + stage1[90:510 - payload_size] + payload + stage1[510:512]
        
        # And write boot sector
        disk_fd.seek(0)
        disk_fd.write(stage1)
        disk_fd.close()

# Dictionary of known implementations
__fs_implementations__ = dict(
    fat1x=FAT1xInstaller,
    fat32=FAT32Installer
)


# Entry point
if __name__ == '__main__':
    opt_parser = optparse.OptionParser(
        description='Install the stage1 into a disk image'
    )
    opt_parser.add_option('-t', '--type', type='str',
        help='Filesystem type', default='fat1x')
    opt_parser.add_option('-d', '--debug', default=False, action='store_true')
    opt_parser.add_option('-L', '--loader-base', type='str',
        help='Loader image location', default='./')
    opts, args = opt_parser.parse_args()
    
    if len(args) < 1:
        opt_parser.error('Missing disk image')
    
    fs_impl = __fs_implementations__.get(opts.type, None)
    if not fs_impl:
        opt_parser.error('Known filesystems: %s' % ', '.join(__fs_implementations__.keys()))
        
    # Setup logging
    if opts.debug:
        log.setLevel(logging.DEBUG)
    log_handler = logging.StreamHandler(sys.stderr)
    log_handler.setFormatter(logging.Formatter('%(levelname)-8s %(message)s'))
    log.addHandler(log_handler)

    fs_impl(args[0], opts.loader_base).install()
    sys.exit(0)
