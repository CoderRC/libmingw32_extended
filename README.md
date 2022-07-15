# libmingw32_extended
A library for mingw32 that includes POSIX functions.

## Build

#### 1. Clone Source
    git clone https://github.com/CoderRC/libmingw32_extended.git
    cd libmingw32_extended

#### 2. Build
    mkdir build
    cd build
    ../configure
    make

## Install
    make install

## Requirements

Download a bash command line environment to run configure.

Download git to use the git command for cloning the source.

Download gcc to compile the source and configure it.

Download make to compile the libary.

If the bash command line environment supports the pacman command do

    pacman -S git
    pacman -S mingw-w64-x86_64-gcc
    pacman -S make
