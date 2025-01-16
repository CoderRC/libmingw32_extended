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

Download make to compile the library.

If the bash command line environment supports the pacman command do

    pacman -S git
    pacman -S mingw-w64-x86_64-gcc
    pacman -S make

## Contributing to the source

To properly add new sources to the repository, the sources must be added to the source directory in the repository and in the configure file add paths to the SOURCES.

To properly add new include directories to the repository, the include directories must be added to the include directory in the repository and in the configure file add include paths to the INCLUDE_DIRECTORIES.

To properly add new headers to the repository, the headers must be added to the include directory in the repository and in the configure file add paths to the INCLUDE_FILES.

## Functions Completed and Can Be Used In Your Projects:
pipe, wait, mmap, munmap, msync, mlock, munlock, posix_madvise, madvise, shm_open, shm_unlink, readv, writev, process_vm_readv, process_vm_writev, dlopen, dlclose, dlsym, dlerror, dladdr