CC=g++
CFLAGS=-Wall

OutPath=$(HOME)/bin/test4MHD

MyIncPath=/usr/local/include
MyLibPath=/usr/local/lib
MyLibsName=microhttpd dl

_I=$(addprefix -I,$(subst :, ,$(MyIncPath)))
_L=$(addprefix -L,$(subst :, ,$(MyLibPath)))
_l=$(addprefix -l,$(subst :, ,$(MyLibsName)))

all:
	$(CC) $(CFLAGS) -o $(OutPath) main.c $(_I) $(_L) $(_l) 

clean:
	rm -rf $(OutPath)
