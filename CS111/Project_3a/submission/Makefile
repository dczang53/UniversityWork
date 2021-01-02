CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: default dist clean

default:
	@$(CC) $(CFLAGS) -o lab3a lab3a.c

dist:
	tar -czf lab3a-704766877.tar.gz lab3a.c ext2_fs.h Makefile README

clean:
	@rm -f lab3a lab3a-704766877.tar.gz
