include Makefile.mk

CXX     = gcc
LINKER  = ld
DEFINES = -DDEBUG

DIRS = "bootloader/stage1" "bootloader/stage2"

# Output
FLOPPY_IMAGE = "build/floppy.img"
MOUNT_POINT  = "/mnt/"

# Boot loader
STAGE1 = "build/stage1.bin"
STAGE2 = "build/stage2.bin"

# Default target
all: build $(STAGE1) $(STAGE2)

build:
	mkdir -p build

# Helper
force_look:
	@true

# Install target
mkfloppy: all
	mkfloppy.sh

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
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean); done

