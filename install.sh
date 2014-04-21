#!/bin/bash
FLOPPY_IMAGE=build/floppy.img
PLATFORM=`uname`

function mkfs_img()
{
    if [ "${PLATFORM}" == "Darwin" ]; then
        device=`hdiutil attach -imagekey diskimage-class=CRawDiskImage -nomount $1`
        echo "Using device ${device}"
        newfs_msdos -v "Boot Disk" -f 1440 ${device}
        hdiutil detach ${device}
    else
        mkfs.vfat "$1"
    fi
}

function mount_img()
{
    if [ ! -f "$2" ]; then
        mkdir -p "$2"
    fi
    if [ ${PLATFORM} == "Darwin" ]; then
        device=`hdiutil attach -imagekey diskimage-class=CRawDiskImage -nomount $1`
        echo "Using device ${device}"
        mount -t msdos ${device} $2
    else
        mount -t vfat -o loop -o umask=000 "$1" "$2"
    fi
}

function umount_img()
{
    if [ ${PLATFORM} == "Darwin" ]; then
        umount $2
        hdiutil detach ${device}
    else
        umount $2
    fi
}

# Create the floppy
if [ ! -f "${FLOPPY_IMAGE}" ]; then
    echo ">> Creating empty image"
    dd if=/dev/zero of="${FLOPPY_IMAGE}" bs=1024 count=1440
    echo ">> Format floppy"
    mkfs_img "${FLOPPY_IMAGE}"
fi

# Copy stage 1
echo ">> Copy boot"
dd if=build/stage1.bin of="${FLOPPY_IMAGE}" conv=notrunc #bs=1 count=450 seek=62 skip=62

# Now, mount the floppy and copy the file
echo ">> Copy boot 2"
mount_img "${FLOPPY_IMAGE}" "build/mount/"
cp build/stage2.bin build/mount/
umount_img "${FLOPPY_IMAGE}" "build/mount/"

echo ">> Done"
