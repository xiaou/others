#! /bin/bash

ARR=$(cut -f1 -d: /etc/passwd | head -4)
echo ${ARR[*]}

echo
for entry in ${ARR[*]}; do
	echo $entry '<--'
done

echo
echo ${#ARR[*]}
echo ${#ARR[0]}

for((i=0;i!=2;i++)); do
	echo '-->' ${ARR[${i}]}
done

echo
i=5
while [ $i -gt 0 ]; do
	echo --- $i
	i=$(($i - 1))
done

echo 
until [ $i = 5 ]; do
	echo --- $i
	i=$(($i + 1))
done

echo
i+=1
case $i in
	5) 
		echo in case:i==5.
		;;
	*) 
		echo in case:i!=5.
		;;
esac


