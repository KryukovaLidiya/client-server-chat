CC=g++
CFLAGS=-Wall -lpthread -lboost_system -std=c++11
LDFLAGS=

all: server client

server: server.o
	$(CC) $(CFLEGS) -o $@ $<

client: client.o
	$(CC) $(CFLAGS) -o $@ $<

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

