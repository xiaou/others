#! /bin/sh

make clean && make
echo
./writer
echo
./reader

