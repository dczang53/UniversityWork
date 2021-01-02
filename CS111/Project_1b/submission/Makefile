CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: default client server clean dist

default:
	@$(CC) $(CFLAGS) -lz -o lab1b-client lab1b-client.c
	@$(CC) $(CFLAGS) -lz -o lab1b-server lab1b-server.c

client:
	@$(CC) $(CFLAGS) -lz -o lab1b-client lab1b-client.c

server:
	@$(CC) $(CFLAGS) -lz -o lab1b-server lab1b-server.c

clean:
	@rm -f lab1b-client lab1b-server lab1b-704766877.tar.gz

dist:
	tar -czf lab1b-704766877.tar.gz lab1b-client.c lab1b-server.c Makefile README
