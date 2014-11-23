dalek
=====
Few years ago I messed around a bit with how to boot a computer. Felt nostalgic and brought it back to life.


## Build

### MacOs X
Using ports, you will need to install cmake, i386-elf-gcc and mtools.
Then, run these commands

mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/modules/Platform/i386.cmake ..

