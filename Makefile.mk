PLATFORM=$(shell uname)
ifeq ($(PLATFORM), Darwin)
    CXX = i386-elf-gcc
    LD = i386-elf-ld
else
    CXX = gcc
    LD  = ld
endif

CFLAGS = -ffreestanding -fno-stack-protector -fno-builtin -nostdinc -Wall -g0 -m32
LDFLAGS = -m elf_i386
ECHO = @echo

