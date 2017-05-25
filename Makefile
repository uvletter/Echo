CFLAGS = -Wall -O2

all:server.c client.c
	cc $(CFLAGS) -o server server.c
	cc $(CFLAGS) -o client client.c
