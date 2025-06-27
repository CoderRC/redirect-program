# redirect-program
A program which redirects to another program.

## Build

#### 1. Clone Source
    git clone https://github.com/CoderRC/redirect-program.git
    cd redirect-program

#### 2. Build
    mkdir build
    cd build
    ../configure "REDIRECT=bin/redirect-program ../a"
    make

## Install
    make install

## Requirements

Download a sh command line environment to run configure.

Download git to use the git command for cloning the source.

Download make to compile the library.

Download gcc with posix c functions to compile the source and configure it.

If the gcc is mingw and the sh command line environment supports the pacman command do

    pacman -S git
    pacman -S make
    pacman -S mingw-w64-x86_64-gcc
    git clone https://github.com/CoderRC/libmingw32_extended.git
    cd libmingw32_extended
    mkdir build
    cd build
    ../configure
    make
    make install
    cd ../..
    git clone https://github.com/CoderRC/redirect-program.git
    cd redirect-program
    mkdir build
    cd build
    ../configure "REDIRECT=bin/redirect-program ../a" LDFLAGS=-lmingw32_extended
    make

If the sh command line environment supports the pacman command do

    pacman -S git
    pacman -S make
    pacman -S gcc

## Contributing to the source

To properly add new sources to the repository, the sources must be added to the source directory in the repository.
