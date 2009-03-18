/*
 * flopy.c
 *
 * Floppy controller
 *
 */
#include <printf.h>
#include <floppy.h>
#include <ports.h>

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

/** Initialize */
void fd_init()
{
  uint8 drives, a, b;
  fdFloppy *fd;

  outportb(0x70, 0x10);
  drives = inportb(0x71);
  a = drives >> 4;
  b = drives & 0xF;

  /* Register drives */
  if(a)
  {
    /* Drive 0 */
    fd       = (fdFloppy*)malloc(sizeof(fdFloppy));
    fd->type = a;
    io_register_device("/floppy/0", drive_types[a], fd);
  }
  if(drives & 0xF)
  {
    /* Drive 1 */
    fd = (fdFloppy*)malloc(sizeof(fdFloppy));
    fd->type = b;
    io_register_device("/floppy/1", drive_types[b], fd);
  }
}
