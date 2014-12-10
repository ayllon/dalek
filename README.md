dalek
=====
Few years ago I messed around a bit with how to boot a computer. Felt nostalgic and brought it back to life.


## Build

### MacOs X
Using ports, you will need to install cmake, i386-elf-gcc and mtools.
Then, run these commands

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/modules/Platform/i386.cmake -DCMAKE_BUILD_TYPE=DEBUG ..
make floppy
```

### Linux
Assuming a 64 bits Linux machine, you will need to install cmake, gcc and mtools.

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/modules/Platform/i386.cmake -DCMAKE_BUILD_TYPE=DEBUG ..
make floppy
```

## Debugging
As a result of the compilation, inside your build directory under `root`, there
will be two files: `stage2.bin` and `stage2.sym`

* `stage2.bin` is the plain binary code that is loaded by `stage1`
* `stage2.sym` contains the debug symbols

`stage2.bin` is copied into `floppy.img`, which can be booted using qemu for debugging as follows

```bash
qemu-system-i386 -fda floppy.img -s -S
```

This will start qemu opening the port 1234 for debugging, and waiting for gdb to connect.
Then, open gdb pointing to the .sym file, and connect to qemu

```bash
gdb root/stage2.sym
(gdb) target remote :1234
Remote debugging using :1234
0x0000fff0 in ?? () 
```

Press c to start the system, and use gdb as usual!

```
(gdb) bt
#0  kb_getc () at .../dalek/bootloader/stage2/keyboard.c:135
#1  0x000011dc in getc () at .../dalek/bootloader/stage2/get.c:16
#2  0x0000120a in gets (dest=0x6d40 <buffer> "a") at .../dalek/bootloader/stage2/get.c:33
#3  0x00000b88 in CLI () at .../dalek/bootloader/stage2/cli.c:50
#4  0x00002153 in main (boot_info=0x641 <bootInformation>) at .../dalek/bootloader/stage2/main.c:44
```

