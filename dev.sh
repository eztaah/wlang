#!/bin/sh
set -e

make debug

echo 
echo "$ ./compiler test.cp -v --build-dir ./build --output ./build/prog"
./compiler test.cp -v --build-dir ./build --output ./build/prog

echo
echo "$ ./build/prog"
./build/prog