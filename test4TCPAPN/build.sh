#! /bin/sh

SH_DIR=
x=`echo $0 | grep "^/"`
if test "${x}"; then
        SH_DIR=$(dirname $0)
else
        SH_DIR=$(dirname `pwd`/$0)
fi
cd $SH_DIR

mkdir BUILD
cd BUILD
cmake ..
make
cd ..
rm -rf log

