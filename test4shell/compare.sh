#! /bin/sh


a=2
b='2'
c=

if [ ! ${a} -eq 0 ]; then
	echo in if1~
fi

if [ $a = $b ]; then
	echo in if2~
fi

if test $a ; then
	echo in if3~
else
	echo in else3~
fi

if [ -z $c ]; then
	echo in if4~
fi

if [ ! -e /xx ]; then
	echo in if-e
fi

if [  ]; then
	:
else if [  ]; then
	:
else if [  ]; then
	:
else
	echo if-else-if..
fi
fi
fi



