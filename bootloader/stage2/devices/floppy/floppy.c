/*
 * flopy.c
 *
 * Floppy controller
 *
 * Based on code from mystran
 *      http://forum.osdev.org/viewtopic.php?t=13538
 * And from Fabian Nunez & Pablo Guerrero Rosel
 *      https://gitorious.org/nopos/nopos/source/kernel/floppy.c
 */
#include <dma.h>
#include <errno.h>
#include <irq.h>
#include <memory.h>
#include <modules.h>
#include <panic.h>
#include <ports.h>
#include <stdio.h>
#include <timer.h>

#include "floppy.h"

static const char *drive_types[] =
{
    "none",
    "360kB 5.25\"",
    "1.2MB 5.25\"",
    "720kB 3.5\"",
    "1.44MB 3.5\"",
    "2.88MB 3.5\"",
    "unknown type",
    "unknown type"
};

static FloppyGeometry drive_geometry[] =
{
    FLOPPY_UNSUPPORTED, // none
    FLOPPY_UNSUPPORTED, // 360k 5.25
    FLOPPY_UNSUPPORTED, // 1.2MB 5.25
    FLOPPY_UNSUPPORTED, // 720k 3.5
    {2, 80, 18, 512},   // 1.44MB 3,5
    FLOPPY_UNSUPPORTED, // 2.88MB 3.5
    FLOPPY_UNSUPPORTED, // unknown
    FLOPPY_UNSUPPORTED  // unknown
};

static volatile uint8_t irq_waiting = 0;

Floppy fd_units[2];

static char fd_dma_buffer[512] __attribute__((aligned(32768))); // 32K

/* IRQ Handler */
void fd_irq_handler(Registers *r)
{
    irq_waiting++;
}

/* Timer task to kill the motor
 */
void fd_timer(int task_id)
{
    register int i;

    for (i = 0; i < sizeof(fd_units) / sizeof(Floppy); i++) {
        // Kill
        if (fd_units[i].motor_state == FD_MOTOR_WAIT) {
            fd_units[i].motor_ticks -= 50;
            if (fd_units[i].motor_ticks <= 0) {
                fd_motor_kill(&fd_units[i]);
            }
        }
    }
}


/**
 * Wait until we get an interrupt
 */
int fd_wait_irq(int timeout)
{
    int start = timer_get_tick();
    while (irq_waiting == 0 && ((timer_get_tick() - start) < timeout)) {
        asm("hlt");
    }
    if (irq_waiting > 0) {
        irq_waiting--;
        return 0;
    }
    else {
        log(LOG_WARN, __func__, "Timeout (%d)", timeout);
        errno = ETIME;
        return -1;
    }
}

/**
 * Bind the methods
 */
static void fd_device_bind_methods(IODevice* fd)
{
    fd->read = fd_read;
    fd->seek = fd_seek;
    fd->write = fd_write;
}


static void fd_send_byte(uint16_t base, uint8_t command)
{
    outportb(base + FD_DATA_FIFO, command);
}


static uint8_t fd_recv_byte(uint16_t base)
{
    return inportb(base + FD_DATA_FIFO);
}


static void fd_configure(uint16_t base, uint8_t seek, uint8_t fifo,
        uint8_t polling, uint8_t threshold, uint8_t precompensation)
{
    fd_send_byte(base, FD_CONFIGURE);
    fd_send_byte(base, 0x00);
    fd_send_byte(base, (seek << 6) | (fifo << 5) | (polling << 4) | (threshold - 1));
    fd_send_byte(base, precompensation);
}

/** Initialize */
int fd_init(void)
{
    uint8_t drives, a, b;
    Floppy *fd;

    memset(fd_units, 0, sizeof(fd_units));
    fd_units[0].drive_number = 0xFF;
    fd_units[1].drive_number = 0xFF;

    /* Check version */
    fd_send_byte(FD_PRIMARY_BASE, FD_VERSION);
    uint8_t version = fd_recv_byte(FD_PRIMARY_BASE);
    if (version != 0x90) {
        log(LOG_WARN, __func__, "Unknown floppy controller version: %x", version);
        return -1;
    }

    /* Register handler */
    irq_install_handler(6, fd_irq_handler);

    /* Ask CMOS for drives */
    outportb(0x70, 0x10);
    drives = inportb(0x71);
    a = drives >> 4;
    b = drives & 0xF;

    /* Reset controller */
    if (fd_reset(FD_PRIMARY_BASE) < 0) {
        log(LOG_ERROR, __func__, "Failed to reset the floppy controller (%d)", errno);
    }

    /* Configure the controller
     * implied seek on, FIFO on, drive polling mode off, threshold = 8, precompensation 0
     */
    fd_configure(FD_PRIMARY_BASE, 1, 1, 0, 8, 0);

    /* Lock so the configuration does not change after reset */
    fd_send_byte(FD_PRIMARY_BASE, FD_LOCK);
    uint8_t locked = fd_recv_byte(FD_PRIMARY_BASE);
    if (locked != 0x10) {
        log(LOG_ERROR, __func__, "Failed to lock the floppy configuration (%x)", locked);
        return -1;
    }

    /* Drive 0 */
    if (a) {
        if (drive_geometry[a].heads != 0) {
            fd = &fd_units[0];
            fd->base = FD_PRIMARY_BASE;
            fd->drive_number = 0;
            fd->motor_state = FD_MOTOR_OFF;
            fd->geometry = drive_geometry[a];
            fd->n_blocks = fd->geometry.spt * fd->geometry.tracks * fd->geometry.heads;
            fd->block = 0;
            IODevice* fd0 = io_register_device("fd0", drive_types[a], fd);
            fd0->id = 0x00; // BIOS drive number
            fd_device_bind_methods(fd0);
        }
        else {
            log(LOG_WARN, __func__, "Unsupported floppy type in slot 0");
        }
    }
    /* Drive 1 */
    if (b) {
        if (drive_geometry[b].heads != 0) {
            fd = &fd_units[1];
            fd->base = FD_PRIMARY_BASE;
            fd->drive_number = 1;
            fd->motor_state = FD_MOTOR_OFF;
            fd->geometry = drive_geometry[b];
            fd->n_blocks = fd->geometry.spt * fd->geometry.tracks * fd->geometry.heads;
            fd->block = 0;
            IODevice* fd1 = io_register_device("fd1", drive_types[b], fd);
            fd1->id = 0x01; // BIOS drive number
            fd_device_bind_methods(fd1);
        }
        else {
            log(LOG_WARN, __func__, "Unsupported floppy type in slot 1");
        }
    }

    // Register timer handler
    timer_register_task(fd_timer, 50);

    log(LOG_INFO, __func__, "Floppy drives initialized");
    return 0;
}

/**
 * Tear down floppy driver
 */
int fd_deinit(void)
{
    // Turn motors off
    outportb(FD_PRIMARY_BASE, 0x0c);
    // Give time to motors to turn off
    sleep(500);
    // Set configuration to the default
    fd_configure(FD_PRIMARY_BASE, 0, 0, 1, 1, 0);
    // Unblock
    fd_send_byte(FD_PRIMARY_BASE, FD_UNLOCK);
    // Reset
    fd_reset(FD_PRIMARY_BASE);
    // Give time to motors to turn off
    sleep(500);
    log(LOG_INFO, __func__, "Floppy module destroyed");
    return 0;
}

MODULE_INIT(fd_init);
MODULE_DESTROY(fd_deinit);


/**
 * Waits until the floppy is ready
 */
static int fd_wait_ready(Floppy *fd)
{
    uint32_t i = 0;
    while (!(0x80 & inportb(fd->base + FD_MAIN_STATUS))) {
        sleep(10); // Sleep 10 milliseconds
        i++;
        // If we have slept 6000 ms, abort
        if (i > 600) {
            errno = EBUSY;
            return -1;
        }
    }
    return 0;
}


static int fd_check_interrupt(Floppy *fd, int *st0, int *cyl)
{
    if (fd_wait_ready(fd) < 0)
        return -1;

    fd_send_byte(fd->base, FD_SENSE_INTERRUPT); // Interrupt received

    fd_wait_ready(fd);
    *st0 = fd_recv_byte(fd->base);
    *cyl = fd_recv_byte(fd->base);
    return 0;
}


int fd_reset(uint16_t base)
{
    outportb(base + FD_DIGITAL_OUTPUT, 0x00); // Disable
    sleep(10); // Give it time
    outportb(base + FD_DIGITAL_OUTPUT, 0x0c); // Enable with motors off
    sleep(10); // Give it time

    // Wait interrupt
    if (fd_wait_irq(2000) < 0)
        return -1;

    // Set transfer speed to 500 Kb/s
    outportb(base + FD_CONFIG_CONTROL, 0x00);

    return 0;
}


int fd_recalibrate(Floppy *fd)
{
    int i, st0, cyl = -1; // set to bogus cylinder
    fd_wait_ready(fd);

    // Dummy seek to force error if no media
    fd_send_byte(fd->base, FD_SEEK);
    fd_send_byte(fd->base, fd->drive_number);
    fd_send_byte(fd->base, 0x01);

    if (fd_wait_irq(5000) < 0)
        return -1;

    if (fd_check_interrupt(fd, &st0, &cyl) < 0)
        return -1;
    if (cyl != 0x1) {
        errno = EAGAIN;
        return -1;
    }

    // 10 attempts
    for (i = 0; i < 10; i++) {
        fd_send_byte(fd->base, FD_RECALIBRATE);
        fd_send_byte(fd->base, fd->drive_number);

        if (fd_wait_irq(5000) < 0)
            return -1;

        if (fd_check_interrupt(fd, &st0, &cyl) < 0)
            return -1;

        if ((st0 & 0xC0) || (st0 & 0x20)) {
            continue;
        }

        if (cyl == 0) { // found cylinder 0 ?
            return 0;
        }
    }

    errno = EAGAIN;
    log(LOG_WARN, __func__, "10 retries exhausted");
    return -1;
}


void fd_motor(Floppy *fd, int stat)
{
    // ON
    if (stat == FD_MOTOR_ON) {
        if (fd->motor_state == FD_MOTOR_OFF) {
            uint8_t dor;
            dor = inportb(fd->base + FD_DIGITAL_OUTPUT);
            dor |= (0x10 << fd->drive_number);
            outportb(fd->base + FD_DIGITAL_OUTPUT, dor);
            sleep(500);
        }
        fd->motor_state = FD_MOTOR_ON;
    }
    // OFF
    else {
        if (fd->motor_state == FD_MOTOR_WAIT) {
            log(LOG_INFO, __func__, "Floppy motor state already waiting...\n");
        }
        fd->motor_ticks = 300; // 3 seconds
        fd->motor_state = FD_MOTOR_WAIT;
    }
}


void fd_motor_kill(Floppy *fd)
{
    uint8_t dor;
    dor = inportb(fd->base + FD_DIGITAL_OUTPUT);
    dor &= (~(0x10 << fd->drive_number));
    outportb(fd->base + FD_DIGITAL_OUTPUT, dor);
    fd->motor_state = FD_MOTOR_OFF;
}


static void fd_block2hcs(FloppyGeometry* geometry, int block, int *head,
        int *track, int *sector)
{
   *head = (block % (geometry->spt * geometry->heads)) / (geometry->spt);
   *track = block / (geometry->spt * geometry->heads);
   *sector = block % geometry->spt + 1;
}


off_t fd_seek(IODevice* self, off_t offset, int whence)
{
    Floppy* fd = (Floppy*)self->data;

    // Only 512 aligned seeks allowed
    if (offset % 512 != 0) {
        log(LOG_ERROR, __func__,
                "Floppy only admits being addressed in multiples of 512 (got %z)",
                offset);
        errno = EINVAL;
        return -1;
    }

    off_t block_offset = offset / 512;

    // Calculate new offset
    uint32_t new_block;
    switch (whence) {
        case SEEK_SET:
            new_block = block_offset;
            break;
        case SEEK_CUR:
            new_block = fd->block + block_offset;
            break;
        default:
            new_block = fd->n_blocks + block_offset;
    }

    // Nothing to do?
    if (fd->block == new_block)
        return block_offset;

    // Beyond the size?
    if (new_block >= fd->n_blocks) {
        errno = EINVAL;
        return -1;
    }

    fd->block = new_block;
    return fd->block;
}


static void fd_setup_dma(Floppy* fd, void* buffer, size_t nbytes, uint8_t write)
{
    union {
        uint8_t  b[4];
        uint32_t v;
    } address, length;

    asm("cli");

    address.v = (uint32_t)buffer;
    length.v  = (uint32_t)nbytes - 1;

    // Buffer address only 24 bits
    if (address.v >> 24)
        panic("%s: Buffer address uses more than 24 bits (%p)", __func__, buffer);
    // nbytes only 16 bits
    if (length.v >> 16)
        panic("%s: DMA length uses more than 16 bits (%z)", __func__, nbytes);
    // Within boundaries
    if (((address.v & 0xFFFF) + length.v) >> 16)
        panic("%s: Buffer address plus length exceeds 16 bits (%p+%d)", __func__, buffer, nbytes);

    uint8_t mode = write ? 0x4A: 0x46;

    outportb(DMA_SINGLE_CHANNEL_MASK, 0x06);        // mask channel 2
    outportb(DMA_FLIP_FLOP, 0xFF);
    outportb(DMA_CHAN2_ADDRESS, address.b[0]);      // buffer low byte
    outportb(DMA_CHAN2_ADDRESS, address.b[1]);      // buffer high byte
    outportb(DMA_CHAN2_PAGE_ADDRESS, address.b[2]); // external page register
    outportb(DMA_FLIP_FLOP, 0xFF);
    outportb(DMA_CHAN2_COUNT, length.b[0]);         // nbytes low byte
    outportb(DMA_CHAN2_COUNT, length.b[1]);         // nbytes high byte
    outportb(DMA_MODE, mode);
    outportb(DMA_SINGLE_CHANNEL_MASK, 0x02);        // unmask channel 2

    asm("sti");
}


static int fd_errno_mapping(int st0, int st1, int st2, int bps)
{
    if (st0 & 0xC0) {
        panic(__func__, "Invalid command sent to the floppy controller");
    }
    else if(st1 & 0x80) {
        log(LOG_ERROR, __func__, "End of cylinder");
        return EIO;
    }
    else if (st0 & 0x08) {
        log(LOG_ERROR, __func__, "Drive not ready");
        return EIO;
    }
    else if (st1 & 0x20) {
        log(LOG_ERROR, __func__, "CRC error");
        return EIO;
    }
    else if (st1 & 0x10) {
        log(LOG_ERROR, __func__, "Controller timeout");
        return EIO;
    }
    else if (st1 & 0x04) {
        log(LOG_ERROR, __func__, "No data found");
        return EIO;
    }
    else if ((st1|st2) & 0x01) {
        log(LOG_ERROR, __func__, "No address mark found");
        return EIO;
    }
    else if (st2 & 0x40) {
        log(LOG_ERROR, __func__, "Deleted address mark");
        return EIO;
    }
    else if(st2 & 0x20) {
        log(LOG_ERROR, __func__, "CRC error in data");
        return EIO;
    }
    else if (st2 & 0x10) {
        log(LOG_ERROR, __func__, "Wrong cylinder");
        return EIO;
    }
    else if (st2 & 0x04) {
        log(LOG_ERROR, __func__, "uPD765 sector not foudn");
        return EIO;
    }
    else if (st2 & 0x02) {
        log(LOG_ERROR, __func__, "Bad cylinder");
        return EIO;
    }
    else if (bps != 0x2) {
        log(LOG_ERROR, __func__, "Wanted 512B/sector, got %d", (1<<(bps+7)));
        return EIO;
    }
    else if (st1 & 0x02) {
        log(LOG_ERROR, __func__, "Not writable");
        return EPERM;
    }
    return 0;
}

// Read or write from or to fd_dma_buffer
static int fd_rw_block(Floppy* fd, uint8_t write)
{
    int head, cyl, sector;

    fd_block2hcs(&fd->geometry, fd->block, &head, &cyl, &sector);

    int cmd = 0xC0;
    if (write)
        cmd |= FD_WRITE_DATA;
    else
        cmd |= FD_READ_DATA;

    int retry;
    for (retry = 0; retry < 10; ++retry) {
        fd_setup_dma(fd, fd_dma_buffer, 512, write);

        fd_send_byte(fd->base, cmd);
        fd_send_byte(fd->base, head<<2|fd->drive_number);
        fd_send_byte(fd->base, cyl);
        fd_send_byte(fd->base, head);
        fd_send_byte(fd->base, sector);
        fd_send_byte(fd->base, 2);    // 512 bytes/sec
        fd_send_byte(fd->base, 1);    // number of tracks
        fd_send_byte(fd->base, FD_GAP3);
        fd_send_byte(fd->base, 0xFF); // unused?

        if (fd_wait_irq(2000) < 0) {
            fd_reset(fd->base);
            return -1;
        }

        // status
        uint8_t st0, st1, st2, bps;
        uint8_t __attribute__((unused)) rcy, rhe, rse;

        st0 = fd_recv_byte(fd->base);
        st1 = fd_recv_byte(fd->base);
        st2 = fd_recv_byte(fd->base);
        rcy = fd_recv_byte(fd->base);
        rhe = fd_recv_byte(fd->base);
        rse = fd_recv_byte(fd->base);
        // bytes per sector
        bps = fd_recv_byte(fd->base);

        errno = fd_errno_mapping(st0, st1, st2, bps);
        if (!errno)
            break;
    }

    if (errno)
        return -1;
    return 0;
}


/** Prepare the floppy for IO
 */
static int fd_activate(Floppy* fd)
{
    // Turn on
    uint8_t dor = inportb(fd->base + FD_DIGITAL_OUTPUT);
    dor &= 0xFC;
    dor |= fd->drive_number;
    outportb(fd->base + FD_DIGITAL_OUTPUT, dor);

    fd_motor(fd, FD_MOTOR_ON);

    // If there was a change, calibrate
    int stat = 0;
    uint8_t dir = inportb(fd->base + FD_DIGITAL_INPUT);
    if (dir & 0x80) {
        log(LOG_INFO, __func__, "Detected a floppy change, try to recalibrate");
        stat = fd_recalibrate(fd);
        if (stat < 0) {
            log(LOG_WARN, __func__, "Failed to recalibrate");
            fd_motor(fd, FD_MOTOR_OFF);
        }
        else {
            log(LOG_INFO, __func__, "Floppy recalibrated");
        }
    }

    return stat;
}

/** Deactivate the floppy
 */
static int fd_deactivate(Floppy* fd)
{
    fd_motor(fd, FD_MOTOR_OFF);
    return 0;
}


ssize_t fd_read(IODevice* self, void* buffer, size_t nbytes)
{
    Floppy* fd = (Floppy*)self->data;

    // Only multiples of 512
    if (nbytes % 512 != 0) {
        log(LOG_ERROR, __func__,
                "Floppy only admits being addressed in multiples of 512 (got %z)",
                nbytes);
        errno = EINVAL;
        return -1;
    }

    size_t nblocks = nbytes / 512;
    size_t i, offset = 0;

    if (fd_activate(fd) < 0)
        return -1;

    // Read full blocks
    for (i = 0; i < nblocks; ++i, offset += 512) {
        if (fd_rw_block(fd, 0) < 0) {
            offset = -1;
            break;
        }
        memcpy(buffer + offset, fd_dma_buffer, 512);
        ++fd->block;
    }

    fd_deactivate(fd);

    return offset;
}


ssize_t fd_write(IODevice* self, const void* buffer, size_t nbytes)
{
    errno = ENOSYS;
    return -1;
}

