#/bin/sh
rm -rf build/*
mkdir -p build
make && ./compiler test.cp --output build/