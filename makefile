CC = gcc
CFLAGS = -Wall -Wextra -g

all: build/server build/client

clean:
	rm -r build/*

build/server: src/c_server/server.c build/protocol_lib.o build/sqlite3.o
	$(CC) $(CFLAGS) -o $@ $^


build/client: src/c_client/client.c build/protocol_lib.o
	$(CC) $(CFLAGS) -o $@ $^



build/protocol_lib.o: src/protocol_lib/protocol_lib.c src/protocol_lib/protocol_lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

build/sqlite3.o: src/c_server/sqlite-amalgamation/sqlite3.c  src/c_server/sqlite-amalgamation/sqlite3.h
	$(CC) $(CFLAGS) -c -o $@ $<