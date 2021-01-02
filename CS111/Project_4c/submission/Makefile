CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: default dist clean

default:
	@$(CC) $(CFLAGS) -lmraa -lm -o lab4c_tcp lab4c_tcp.c
	@$(CC) $(CFLAGS) -lmraa -lm -lssl -lcrypto -o lab4c_tls lab4c_tls.c

dist:
	tar -czf lab4c-704766877.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README

clean:
	@rm -f lab4c_tcp lab4c_tls lab4c-704766877.tar.gz
