PROGS	= TinyServer
OBJS	= config.o
FLAGS	= -Wall
CC	= gcc

ALL: $(OBJS) $(PROGS)

%.o: %.c %.h
	$(CC) -c $^ $(FLAGS)
TinyServer: TinyServer.o $(OBJS)
	$(CC) -o $@ $^ $(FLAGS)

clean:
	$(RM) $(OBJS) $(PROGS) $(wildcard *.h.gch) $(wildcard *.o)
