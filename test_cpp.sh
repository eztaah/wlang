#!/bin/sh

# rm -rf build/*
mkdir -p build
make && ./compiler test.cp --cpp --output build/
g++ -o build/output build/output.cpp
./build/output