#! /bin/bash

echo $0

ARR=(1 2 3 4 5)
LEN=${#ARR[*]}  # '#'就是用来取长度的.

echo ${ARR[*]}
echo LEN=$LEN

echo add:
ARR[5]=6
echo ${ARR[*]}

echo "change index 3:"
ARR=(${ARR[*]/$((3+1))/44})
echo ${ARR[*]}

echo "from index 2, len = 3:"
ARR2=(${ARR[*]:2:3})
echo ${ARR2[*]}

echo erase index 0:
unset ARR[0]
echo ${ARR[*]}
echo erase all:
unset ARR
echo ${ARR[*]}
