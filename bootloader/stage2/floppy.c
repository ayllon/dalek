/*
 * flopy.c
 *
 * Floppy controller
 *
 */
#include <printf.h>
#include <floppy.h>
#include <ports.h>
#include <irq.h>
#include <memory.h>
#include <panic.h>
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
        asm("hlt");
    irq_waiting--;
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

    /* Register drives */
    if (a) {
        /* Drive 0 */
        fd = (Floppy*) malloc(sizeof(Floppy));
        /* Read parameters table */
        fd->base = FD_PRIMARY_BASE;
        fd->motor_state = FD_MOTOR_OFF;
        fd_reset(fd);
        io_register_device("/floppy/0", drive_types[a], fd);
    }
    if (drives & 0xF) {
        /* Drive 1 */
        fd = (Floppy*) malloc(sizeof(Floppy));
        fd->base = FD_SECONDARY_BASE;
        fd->motor_state = FD_MOTOR_OFF;
        fd_reset(fd);
        io_register_device("/floppy/1", drive_types[b], fd);
    }

    // Register timer handler
    timer_register_task(fd_timer, 50);
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
        if (i > 600) // If we have sleeped 6000 ms, abort
            panic("[fd_wait_ready()] Floppy timeout\n");
    }
}

/**
 * Sends a command
 */
void fd_write_command(Floppy *f, uint8_t command)
{
    fd_wait_ready(f);
    outportb(f->base + FD_DATA_FIFO, command);
}

/**
 * Read data from the controller
 */
uint8_t fd_read_data(Floppy *f)
{
    fd_wait_ready(f);
    return inportb(f->base + FD_DATA_FIFO);
}

/**
 */
void fd_check_interrupt(Floppy *f, int *st0, int *cyl)
{
    fd_write_command(f, FD_SENSE_INTERRUPT); // Interrupt received

    *st0 = fd_read_data(f);
    *cyl = fd_read_data(f);
}

/**
 * Resets the controller
 */
int fd_reset(Floppy *f)
{
    int st0, cyl;

    outportb(f->base + FD_DIGITAL_OUTPUT, 0x00); // Disable
    outportb(f->base + FD_DIGITAL_OUTPUT, 0x0C); // Enable
    // Wait interrupt
    fd_wait_irq();
    // Check interrupt
    fd_check_interrupt(f, &st0, &cyl);
    // Set transfer speed to 500 Kb/s
    outportb(f->base + FD_CONFIG_CONTROL, 0x00);

    // Set steprate, head unload and head load
    fd_write_command(f, FD_SPECIFY);
    fd_write_command(f, 0xDF); // steprate = 3ms, unload time = 240ms
    fd_write_command(f, 0x03); // load time = 16ms, no-DMA = 1;

    // Fail?
    if (fd_calibrate(f))
        return -1;

    return 0;
}

/**
 * Move to cylinder 0, which calibrates the drive
 */
int fd_calibrate(Floppy *f)
{
    int i, st0, cyl = -1; // set to bogus cylinder

    fd_motor(f, FD_MOTOR_ON);

    // 10 attemps
    for (i = 0; i < 10; i++) {

        fd_write_command(f, FD_RECALIBRATE);
        fd_write_command(f, (f->base == FD_PRIMARY_BASE) ? 0 : 1); // argument is drive

        fd_wait_irq();
        fd_check_interrupt(f, &st0, &cyl);

        if (st0 & 0xC0) {
            static const char * status[] = { 0, "error", "invalid", "drive" };
            printf("fd_calibrate: Status = %s\n", status[st0 >> 6]);
            continue;
        }

        if (!cyl) { // found cylinder 0 ?
            fd_motor(f, FD_MOTOR_OFF);
            return 0;
        }
    }

    printf("floppy_calibrate: 10 retries exhausted\n");
    fd_motor(f, FD_MOTOR_OFF);
    return -1;
}

/**
 * Changes the status of the motor
 */
void fd_motor(Floppy *f, int stat)
{
    // ON
    if (stat == FD_MOTOR_ON) {
        if (f->motor_state == FD_MOTOR_OFF) {
            // Turn on
            outportb(f->base + FD_DIGITAL_OUTPUT, 0x1C);
            sleep(500);
        }
        f->motor_state = FD_MOTOR_ON;
    }
    // OFF
    else {
        if (f->motor_state == FD_MOTOR_WAIT) {
            printf("fd_motor: Floppy motor state already waiting...\n");
        }
        f->motor_ticks = 300; // 3 seconds
        f->motor_state = FD_MOTOR_WAIT;
    }
}

/* void fd_motor_kill(fdFloppy *f)
 * Kills the floppy motor
 */
void fd_motor_kill(Floppy *f)
{
    outportb(f->base + FD_DIGITAL_OUTPUT, 0x0C);
    f->motor_state = FD_MOTOR_OFF;
}
