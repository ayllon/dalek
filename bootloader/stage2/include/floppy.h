/*
 * floppy.h
 *
 * Floppy driver
 *
 */
#ifndef __FLOPY_H__
#define __FLOPPY_H__

#include <types.h>
#include <io.h>

/* Floppy parameter table */
#define FD_PARAMETER_ADDRESS 0x000fefc7

/* Controllers */
#define FD_PRIMARY_BASE     0x03F0
#define FD_SECONDARY_BASE   0x0370
#define FD_IRQ              6

/* Registers IO ports */
#define FD_STATUS_A         0x00 // read-only
#define FD_STATUS_B         0x01 // read-only
#define FD_DIGITAL_OUTPUT   0x02
#define FD_TAPE_DRIVER      0x03
#define FD_MAIN_STATUS      0x04 // read-only
#define FD_DATA_RATE_SELECT 0x04 // write-only
#define FD_DATA_FIFO        0x05
#define FD_DIGITAL_INPUT    0x07 // read-only
#define FD_CONFIG_CONTROL   0x07 // read-only

/* Commands */
#define FD_READ_TRACK         2
#define FD_SPECIFY            3
#define FD_SENSE_DRIVE_STATUS 4
#define FD_WRITE_DATA         5
#define FD_READ_DATA          6
#define FD_RECALIBRATE        7
#define FD_SENSE_INTERRUPT    8
#define FD_WRITE_DELETED_DATA 9
#define FD_READ_ID            10
#define FD_READ_DELETED_DATA  12
#define FD_FORMAT_TRACK       13
#define FD_SEEK               15
#define FD_VERSION            16
#define FD_SCAN_EQUAL         17
#define FD_PERPENDICULAR_MODE 18
#define FD_CONFIGURE          19
#define FD_VERIFY             22
#define FD_SCAN_LOW_OR_EQUAL  25
#define FD_SCAN_HIGH_OR_EQUAL 29

/* Gap */
#define FD_GAP3               27

/* Motor */
#define FD_MOTOR_OFF  0x00
#define FD_MOTOR_ON   0x01
#define FD_MOTOR_WAIT 0x02

/* Floppy types */
#define FD_NONE      0
#define FD_5_25_360  1
#define FD_5_25_1_2  2
#define FD_3_5_720   3
#define FD_3_5_1_44  4
#define FD_3_5_2_88  5
#define FD_UNKNOWN   6
#define FD_UNKNOWN2  7

/* Floppy structure */
typedef struct {
    uint32_t heads;
    uint32_t tracks;
    uint32_t spt; // sectors per track
    uint32_t block_size;
} FloppyGeometry;
#define FLOPPY_UNSUPPORTED {0, 0, 0, 0}

typedef struct {
    FloppyGeometry geometry;
    uint16_t motor_state;
    uint16_t motor_ticks;
    uint32_t base;
    uint8_t  drive_number;
    uint32_t n_blocks;
    // Position in blocks
    uint32_t block;
} Floppy;


/**
 * Initializes drivers
 */
void fd_init();

/**
 * Sends a command
 */
void fd_send_byte(Floppy *f, uint8_t command);

/**
 * Resets the controller
 */
int fd_reset(Floppy *f);

/**
 * Move to cylinder 0, which calibrates the drive
 * Initializes drivers
 */
int fd_calibrate(Floppy *drive);

/**
 * Changes the status of the motor
 */
void fd_motor(Floppy *drive, int stat);

/**
 * Kills the floppy motor
 */
void fd_motor_kill(Floppy *f);

/**
 * IO methods
 */
off_t   fd_seek(IODevice* self, off_t offset, int whence);
ssize_t fd_read(IODevice* self, void* buffer, size_t nbytes);
ssize_t fd_write(IODevice* self, void* buffer, size_t nbytes);
int     fd_ioctl(IODevice* self, unsigned long request);

#endif
