#!/bin/sh
set -e

make debug

echo 
echo "$ ./compiler test.w -b ./build -o ./build/prog --dev-mode"
./compiler test.cp -b build -o build/prog

echo
echo "$ ./build/prog"
./build/prog