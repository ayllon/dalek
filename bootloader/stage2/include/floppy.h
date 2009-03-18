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
typedef struct
{
  uint8 steprate_headunload;
  uint8 headload_ndma;
  uint8 motor_delay_off; /*specified in clock ticks*/
  uint8 bytes_per_sector;
  uint8 sectors_per_track;
  uint8 gap_length;
  uint8 data_length; /*used only when bytes per sector == 0*/
  uint8 format_gap_length;
  uint8 filler;
  uint8 head_settle_time; /*specified in milliseconds*/
  uint8 motor_start_time; /*specified in 1/8 seconds*/
}floppyParameters;

typedef struct
{
  floppyParameters parameters;
  uint32 base;
}fdFloppy;

/* Functions */

void fd_init();

/* void fd_write_command(fdFloppy *f, uint8 command)
 * Sends a command
 */
void fd_write_command(fdFloppy *f, uint8 command);

/* void fd_reset(fdFloppy *f)
 * Resets the controller
 */
void fd_reset(fdFloppy *f);



void fd_motor_off(IODevice *drive);
void fd_motor_on(IODevice *drive);

void fd_recalibrate(IODevice *drive);
void fd_seek(IODevice *drive, uint32 lba);



#endif
