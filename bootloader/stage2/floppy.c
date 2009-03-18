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

static uint8 irq_waiting = 0;

/* IRQ Handler */
void fd_irq_handler(struct regs *r)
{
  irq_waiting++;
}

/* void fd_wait_irq()
 * Wait until we get an interrupt
 */
void fd_wait_irq()
{
  while(irq_waiting == 0)
    asm("hlt");
  irq_waiting--;    
}

/** Initialize */
void fd_init()
{
  uint8 drives, a, b;
  fdFloppy *fd;

  /* Register handler */
  irq_install_handler(6, fd_irq_handler);

  /* Look for drives */
  outportb(0x70, 0x10);
  drives = inportb(0x71);
  a = drives >> 4;
  b = drives & 0xF;

  /* Register drives */
  if(a)
  {
    /* Drive 0 */
    fd       = (fdFloppy*)malloc(sizeof(fdFloppy));
    /* Read parameters table */
    fd->base = FD_PRIMARY_BASE;
    fd_reset(fd);
    io_register_device("/floppy/0", drive_types[a], fd);
  }
  if(drives & 0xF)
  {
    /* Drive 1 */
    fd = (fdFloppy*)malloc(sizeof(fdFloppy));
    io_register_device("/floppy/1", drive_types[b], fd);
  }
}


/* void fd_wait_ready(fdFloppy *f)
 * Waits until the floppy is ready
 */
void fd_wait_ready(fdFloppy *f)
{
  uint32 i = 0;

  while(!(0x80 & inportb(f->base + FD_MAIN_STATUS)))
  {
    sleep(10); // Sleep 10 milliseconds
    i++;
    if(i > 600) // If we have sleeped 6000 ms, abort
      panic("[fd_wait_ready()] Floppy timeout\n");
  }
}

/* void fd_write_command(fdFloppy *f, uint8 command)
 * Sends a command
 */
void fd_write_command(fdFloppy *f, uint8 command)
{
  fd_wait_ready(f);
  outportb(f->base + FD_DATA_FIFO, command);
}

/* uint8 fd_read_data(fdFloppy *f)
 * Read data from the controller
 */
uint8 fd_read_data(fdFloppy *f)
{
  fd_wait_ready(f);
  return inportb(f->base + FD_DATA_FIFO);
}

/* void fd_check_interrupt(fdFloppy *f, int *st0, int *cyl)
 */
void fd_check_interrupt(fdFloppy *f, int *st0, int *cyl)
{
  fd_write_command(f->base, FD_SENSE_INTERRUPT); // Interrupt received

  *st0 = fd_read_data(f);
  *cyl = fd_read_data(f);
}

/* void fd_reset(fdFloppy *f)
 * Resets the controller
 */
void fd_reset(fdFloppy *f)
{
  struct regs r;
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

  // TODO: Calibrate
}
