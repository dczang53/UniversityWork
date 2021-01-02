CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: default clean dist

default:
	@$(CC) $(CFLAGS) -o lab1a lab1a.c

clean:
	@rm -f lab1a lab1a-704766877.tar.gz

dist:
	tar -czf lab1a-704766877.tar.gz lab1a.c Makefile README
