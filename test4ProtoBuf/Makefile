CC=g++
CFLAGS=-Wall

MyIncPath=/usr/local/include
MyLibPath=/usr/local/lib
MyLibsName=

_I=$(addprefix -I,$(subst :, ,$(MyIncPath)))
_L=$(addprefix -L,$(subst :, ,$(MyLibPath)))
_l=$(addprefix -l,$(subst :, ,$(MyLibsName)))
_ProtoBuf=$$(pkg-config --cflags --libs protobuf)

all:
	$(CC) $(CFLAGS) -o writer writer.cpp helloworld.message/helloworld.Iam.pb.cc $(_I) $(_L) $(_l) $(_ProtoBuf)
	$(CC) $(CFLAGS) -o reader reader.cpp helloworld.message/helloworld.Iam.pb.cc $(_I) $(_L) $(_l) $(_ProtoBuf)

clean:
	rm writer
	rm reader
	rm msg.txt
	
