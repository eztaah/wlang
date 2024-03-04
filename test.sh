#!/bin/sh
set -e

rm -rf build/*
mkdir -p build
make
./compiler test.cp -v --asm

nasm -f elf64 ./build/output.asm -o ./build/output.o
ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o ./build/prog build/output.o -lc
./build/prog