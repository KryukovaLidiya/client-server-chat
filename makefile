SVR_OBJECT_FILES = server.o
CLT_OBJECT_FILES = client.o

CFLAGS=-Wall -std=c++11
LIBS = -lpthread -lboost_system 
CC = g++

all: client server
client: $(CLT_OBJECT_FILES) 
server: $(SVR_OBJECT_FILES)  

client server:
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@  

.cpp.o:
	$(CC) $(CFLAGS) $(LIBS) -c $< 

clean: 
	rm -rf client server *.o
