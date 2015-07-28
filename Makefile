all: TinyServer

TinyServer: TinyServer.o config.o
	gcc -g -o $@ $^

TinyServer.o: TinyServer.c
	gcc -c -g TinyServer.c

config.o: config.c config.h
	gcc -c -g config.c
clean:
	-rm *.o TinyServer
