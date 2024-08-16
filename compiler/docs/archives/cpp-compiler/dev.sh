#!/bin/sh
set -e

make debug

echo 
echo "$ ./compiler test.w -b ./build -o ./build/prog --dev-mode"
./compiler test.w -b ./build -o ./build/prog --dev-mode

echo
echo "$ ./build/prog"
./build/prog