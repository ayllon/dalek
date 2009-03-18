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

/* Registers IO ports */
#define FD_STATUS_A         0x3F0 // read-only
#define FD_STATUS_B         0x3F1 // read-only
#define FD_DIGITAL_OUTPUT   0x3F2
#define FD_TAPE_DRIVER      0x3F3
#define FD_MAIN_STATUS      0x3F4 // read-only
#define FD_DATA_RATE_SELECT 0x3F4 // write-only
#define FD_DATA_FIFO        0x3F5
#define FD_DIGITAL_INPUT    0x3F7 // read-only
#define FD_CONFIG_CONTROL   0x3F7 // read-only

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

/* Floppy types */
#define FD_NONE 0
#define FD_5_25_360  1
#define FD_5_25_1_2  2
#define FD_3_5_720   3
#define FD_3_5_1_44  4
#define FD_3_5_2_88  5
#define FD_UNKNOWN   6
#define FD_UNKNOWN2  7

/* Floppy structure */
typedef struct
{
  uint8 type;
}fdFloppy;

/* Functions */

void fd_init();

void fd_motor_off(IODevice *drive);
void fd_motor_on(IODevice *drive);

void fd_recalibrate(IODevice *drive);
void fd_seek(IODevice *drive, uint32 lba);

#endif
