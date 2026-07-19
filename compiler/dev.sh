#!/bin/sh

set -xe

clear
make clean 
make debug
./wlangc ./test.w -vde
