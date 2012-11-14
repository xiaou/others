#! /bin/sh

printf "\necho in %s :\n" $0

V1=var1 
export V2=var2

echo V1: ${V1}
echo export V2: ${V2}

func(){
	printf "\necho in %s's func :\n" $0
    local V1=var1_local
	export V3=var3
	echo local V1: ${V1}
	echo export V3: ${V3}
}

func

printf "\necho after func() in %s :\n" $0
echo V1 = ${V1}

func2(){ 
	printf "\necho in %s's func2 :\n" $0  
}

./funcAndVal2.sh

