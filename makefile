CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: build/server build/client

clean:
	rm -r build

build/server: src/c_server/server.c
	$(CC) $(CFLAGS) -o $@ $^

build/client: src/c_client/client.c
	$(CC) $(CFLAGS) -o $@ $^