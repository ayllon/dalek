/*
 * flopy.c
 *
 * Floppy controller
 *
 */
#include <errno.h>
#include <floppy.h>
#include <irq.h>
#include <memory.h>
#include <panic.h>
#include <ports.h>
#include <printf.h>
#include <timer.h>


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
    FLOPPY_UNSUPPORTED,         // none
    FLOPPY_UNSUPPORTED,         // 360k 5.25
    FLOPPY_UNSUPPORTED,         // 1.2MB 5.25
    FLOPPY_UNSUPPORTED,         // 720k 3.5
    {2, 80, 18, 512, 1474560},  // 1.44MB 3,5
    FLOPPY_UNSUPPORTED,         // 2.88MB 3.5
    FLOPPY_UNSUPPORTED,         // unknown
    FLOPPY_UNSUPPORTED          // unknown
};

static uint8_t irq_waiting = 0;

Floppy fd_units[2];

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
void fd_wait_irq()
{
    while (irq_waiting == 0)
        asm("nop");
    irq_waiting--;
}

/**
 * Bind the methods
 */
static void fd_device_bind_methods(IODevice* fd)
{
    fd->read = fd_read;
    fd->seek = fd_seek;
    fd->write = fd_write;
    fd->ioctl = fd_ioctl;
}

/** Initialize */
void fd_init()
{
    uint8_t drives, a, b;
    Floppy *fd;

    /* Register handler */
    irq_install_handler(6, fd_irq_handler);

    /* Look for drives */
    outportb(0x70, 0x10);
    drives = inportb(0x71);
    a = drives >> 4;
    b = drives & 0xF;

    /* Drive 0 */
    if (a) {
        if (drive_geometry[a].heads != 0) {
            fd = (Floppy*) malloc(sizeof(Floppy));
            fd->base = FD_PRIMARY_BASE;
            fd->motor_state = FD_MOTOR_OFF;
            fd->geometry = drive_geometry[a];
            fd->drive_number = 0;
            fd_reset(fd);
            IODevice* fd0 = io_register_device("fd0", drive_types[a], fd);
            fd_device_bind_methods(fd0);
        }
        else {
            log(LOG_WARN, __func__, "Unsupported floppy type in slot 0");
        }
    }
    /* Drive 1 */
    if (b) {
        if (drive_geometry[b].heads != 0) {
            fd = (Floppy*) malloc(sizeof(Floppy));
            fd->base = FD_PRIMARY_BASE;
            fd->motor_state = FD_MOTOR_OFF;
            fd->geometry = drive_geometry[b];
            fd->drive_number = 1;
            fd_reset(fd);
            IODevice* fd1 = io_register_device("fd1", drive_types[b], fd);
            fd_device_bind_methods(fd1);
        }
        else {
            log(LOG_WARN, __func__, "Unsupported floppy type in slot 1");
        }
    }

    // Register timer handler
    timer_register_task(fd_timer, 50);

    log(LOG_INFO, __func__, "Floppy drives initialized");
}


/**
 * Waits until the floppy is ready
 */
void fd_wait_ready(Floppy *f)
{
    uint32_t i = 0;
    while (!(0x80 & inportb(f->base + FD_MAIN_STATUS))) {
        sleep(10); // Sleep 10 milliseconds
        i++;
        if (i > 600) // If we have slept 6000 ms, abort
            panic("[fd_wait_ready()] Floppy timeout\n");
    }
}


void fd_send_byte(Floppy *fd, uint8_t command)
{
    outportb(fd->base + FD_DATA_FIFO, command);
}


uint8_t fd_recv_byte(Floppy *fd)
{
    return inportb(fd->base + FD_DATA_FIFO);
}


void fd_check_interrupt(Floppy *fd, int *st0, int *cyl)
{
    fd_wait_ready(fd);
    fd_send_byte(fd, FD_SENSE_INTERRUPT); // Interrupt received

    fd_wait_ready(fd);
    *st0 = fd_recv_byte(fd);
    *cyl = fd_recv_byte(fd);
}


int fd_reset(Floppy *fd)
{
    int st0, cyl;

    outportb(fd->base + FD_DIGITAL_OUTPUT, 0x00 | fd->drive_number); // Disable
    outportb(fd->base + FD_DIGITAL_OUTPUT, 0x0C | fd->drive_number); // Enable
    // Wait interrupt
    fd_wait_irq();
    // Check interrupt
    fd_check_interrupt(fd, &st0, &cyl);
    // Set transfer speed to 500 Kb/s
    outportb(fd->base + FD_CONFIG_CONTROL, 0x00);

    // Set steprate, head unload and head load
    fd_wait_ready(fd);
    fd_send_byte(fd, FD_SPECIFY);
    fd_send_byte(fd, 0xDF); // steprate = 3ms, unload time = 240ms
    fd_send_byte(fd, 0x03); // load time = 16ms, no-DMA = 1;

    // Fail?
    if (fd_calibrate(fd))
        return -1;

    return 0;
}


int fd_calibrate(Floppy *fd)
{
    int i, st0, cyl = -1; // set to bogus cylinder

    fd_motor(fd, FD_MOTOR_ON);

    // 10 attempts
    for (i = 0; i < 10; i++) {
        fd_wait_ready(fd);
        fd_send_byte(fd, FD_RECALIBRATE);
        fd_send_byte(fd, (fd->base == FD_PRIMARY_BASE) ? 0 : 1); // argument is drive

        fd_wait_irq();
        fd_check_interrupt(fd, &st0, &cyl);

        if (st0 & 0xC0) {
            static const char * status[] = { 0, "error", "invalid", "drive" };
            printf("fd_calibrate: Status = %s\n", status[st0 >> 6]);
            continue;
        }

        if (!cyl) { // found cylinder 0 ?
            fd_motor(fd, FD_MOTOR_OFF);
            return 0;
        }
    }

    printf("floppy_calibrate: 10 retries exhausted\n");
    fd_motor(fd, FD_MOTOR_OFF);
    return -1;
}


void fd_motor(Floppy *fd, int stat)
{
    // ON
    if (stat == FD_MOTOR_ON) {
        if (fd->motor_state == FD_MOTOR_OFF) {
            // Turn on
            outportb(fd->base + FD_DIGITAL_OUTPUT, 0x1C | fd->drive_number);
            sleep(500);
        }
        fd->motor_state = FD_MOTOR_ON;
    }
    // OFF
    else {
        if (fd->motor_state == FD_MOTOR_WAIT) {
            printf("fd_motor: Floppy motor state already waiting...\n");
        }
        fd->motor_ticks = 300; // 3 seconds
        fd->motor_state = FD_MOTOR_WAIT;
    }
}


void fd_motor_kill(Floppy *fd)
{
    outportb(fd->base + FD_DIGITAL_OUTPUT, 0x0C | fd->drive_number);
    fd->motor_state = FD_MOTOR_OFF;
}


static void fd_block2hcs(FloppyGeometry* geometry, int block, int *head,
        int *track, int *sector)
{
   *head = (block % (geometry->spt * geometry->heads)) / (geometry->spt);
   *track = block / (geometry->spt * geometry->heads);
   *sector = block % geometry->spt + 1;
}


static int fd_physical_seek(Floppy *fd, int head, int cyl)
{
    fd_wait_ready(fd);
    fd_send_byte(fd, FD_SEEK);
    fd_send_byte(fd, head << 2);
    fd_send_byte(fd, cyl);

    int st0, resulting_cyl;
    fd_check_interrupt(fd, &st0, &resulting_cyl);
    if (st0 != 0x20 || (resulting_cyl != cyl)) {
        errno = EIO;
        return -1;
    }
    return 0;
}


off_t fd_seek(IODevice* self, off_t offset, int whence)
{
    Floppy* fd = (Floppy*)self->data;

    // Convert between block + offset into total offset
    off_t current_position = (fd->block * fd->geometry.block_size)
            + fd->block_offset;

    // Calculate new offset
    off_t new_position;
    switch (whence) {
        case SEEK_SET:
            new_position = offset;
            break;
        case SEEK_CUR:
            new_position = current_position + offset;
            break;
        default:
            errno = ENOSYS;
            return -1;
    }

    // Nothing to do?
    if (current_position == new_position)
        return current_position;

    // Beyond the size?
    if (new_position >= fd->geometry.size) {
        errno = EINVAL;
        return -1;
    }

    // Calculate new block, and offset within the block
    fd->block = new_position / 512;
    fd->block_offset = new_position % 512;

    return new_position;
}


ssize_t fd_read(IODevice* self, void* buffer, size_t nbytes)
{
    Floppy* fd = (Floppy*)self->data;
    int head, cyl, sector;

    fd_block2hcs(&fd->geometry, fd->block, &head, &cyl, &sector);

    fd_motor(fd, FD_MOTOR_ON);

    int retry;
    for (retry = 0; retry < 10; ++retry) {
        if (fd_physical_seek(fd, head, cyl) < 0) {
            fd_motor(fd, FD_MOTOR_OFF);
            log(LOG_WARN, __func__, "Failed to seek to block %d", fd->block);
            return -1;
        }

        //dma_xfer(2, tbaddr, 512);
    }

    return 0;
}


ssize_t fd_write(IODevice* self, void* buffer, size_t nbytes)
{
    errno = ENOSYS;
    return -1;
}


int fd_ioctl(IODevice* self, unsigned long request)
{
    errno = ENOSYS;
    return -1;
}

