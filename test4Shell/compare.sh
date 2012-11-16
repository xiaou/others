#! /bin/sh


a=2
b='2'
c=

if [ ! ${a} -eq 0 ]; then
	echo in if1
fi

if [ $a = $b ]; then
	echo in if2
fi

if test $c ; then
	echo in if3
else
	echo in else3
fi

if [ -z $c ]; then
	echo in if4
fi

if [ ! -e /xx ]; then
	echo in if-5
fi

if [  ]; then
	:
else if [  ]; then
	:
else if [  ]; then
	:
else
	echo in if-else-if-else-6..
fi
fi
fi

if [ $a = $b ] && ( test $c || [ -z $c ] ); then
	echo in if-7
fi



