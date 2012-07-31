
CC=g++
CFLAGS=-Wall

OutPath4UDPClient=$(HOME)/bin/test4UDPClient
OutPath4UDPServer=$(HOME)/bin/test4UDPServer

MyLibsName=pthread
_l=$(addprefix -l,$(subst :, ,$(MyLibsName)))

all:
	$(CC) $(CFLAGS) -o $(OutPath4UDPClient) client_main.c
	$(CC) $(CFLAGS) -o $(OutPath4UDPServer) server_main.c $(_l)

clean:
	rm -rf $(OutPath4UDPClient)
	rm -rf $(OutPath4UDPServer)


