#! /bin/sh

printf "\necho in %s :\n" $0

echo V1:${V1}
echo V2:${V2}
echo V3:${V3}

printf "\n-------------freedom------------\n\n"

func(){
	printf "\necho in %s's func :\n" $0
	echo '$0:' $0
	echo '$1:' $1
	echo '$2:' $2
	return $(($1 + $2))
}

func 4 1
RES=$? # 函数返回值在 $? 里.
echo RES: ${RES}

printf " \ninvoke funcs :\n"
. ./funcs.inc
funcs_func
func
func2

func2(){
	echo 
}
func2

