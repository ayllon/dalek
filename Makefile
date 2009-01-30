include Makefile.inc

CXX     = gcc
LINKER  = ld
DEFINES = -DDEBUG

DIRS = "bootloader/stage1" "bootloader/stage2"

# Output
FLOPPY_IMAGE = "/home/alejandro/DesarrolloHost/dalek/floppy.img"
MOUNT_POINT  = "/mnt/"

# Boot loader
STAGE1 = "build/stage1.bin"
STAGE2 = "build/stage2.bin"

# Default target
all: $(STAGE1) $(STAGE2)


# Helper
force_look:
	@true

# Install target
install: all
	# Copy stage 1
	# In the future it would be a nice idea to keep
	# the disk header (maybe a installer)
	dd if=$(STAGE1) of=$(FLOPPY_IMAGE) conv=notrunc

	# Now, mount the floppy (need sudo)
	#sudo mount -t vfat -o loop -o umask=000 $(FLOPPY_IMAGE) $(MOUNT_POINT)

	# Copy stage 2
	#cp $(STAGE2) $(MOUNT_POINT)

	# Umount
	#sudo umount $(MOUNT_POINT)

# Boot loader (Stage1)
$(STAGE1): force_look
	$(ECHO) Building stage1 boot loader
	cd bootloader/stage1; $(MAKE) $(MFLAGS)

# Boot loader (Stage 2)
$(STAGE2): force_look
	$(ECHO) Building stage2 boot loader
	cd bootloader/stage2; $(MAKE) $(MFLAGS)

clean:
	$(ECHO) Cleaning...
	-for d in ($DIRS); do (cd $$d; $(MAKE) clean); done

