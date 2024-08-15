#!/bin/sh

set -xe

clear
make clean 
make debug
./wlangc ./test/test.w -vde
