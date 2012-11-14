#! /bin/sh

v1=val1
r1=v1
eval v2='$'$r1
echo $v1
echo $v2

exit

x=4

x=$(expr ${x} + 1)

echo ${x}
