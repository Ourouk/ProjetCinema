CC = gcc
CFLAGS = -Wall -Wextra -g -lssl -lcrypto

all: target/server target/client

clean:
	rm -r target/*

target/server: src/c_server/server.c target/protocol_lib.o target/sqlite3.o
	$(CC) $(CFLAGS) -o $@ $^


target/client: src/c_client/client.c target/protocol_lib.o
	$(CC) $(CFLAGS) -o $@ $^



target/protocol_lib.o: src/protocol_lib/protocol_lib.c src/protocol_lib/protocol_lib.h
	$(CC) $(CFLAGS) -c -o $@ $<

target/sqlite3.o: src/c_server/sqlite-amalgamation/sqlite3.c  src/c_server/sqlite-amalgamation/sqlite3.h
	$(CC) $(CFLAGS) -c -o $@ $<