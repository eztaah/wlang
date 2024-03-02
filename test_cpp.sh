#!/bin/sh

make && ./compiler test.cp --cpp --output build/output.cpp
g++ -o build/output build/output.cpp
./build/output