#! /bin/bash

ARR=(1 2 3 4)
ARR[4]=5
len=
echo ${ARR[@]}
echo ${ARR[0]}

echo
for i in ${ARR[@]}; do
	echo $i
done

echo
for((i=0;i!=3;i++)); do
	echo ${ARR[${i}]}
done

echo
unset ARR[*]
echo ${#a}
for entry in $(cut -f1,3 -d: /etc/passwd | head -10); do
	ARR[ ${entry#*:}  ]=${entry%:*}
done
echo ${ARR[@]}

echo ${#ARR}

