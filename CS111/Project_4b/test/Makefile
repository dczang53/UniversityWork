CC = gcc
CFLAGS = -Wall -Wextra -lmraa -lm

.PHONY = default check clean dist

default: lab4b.c

	@find . -exec touch {} \;
	@$(CC) $(CFLAGS) -o lab4b lab4b.c

check: lab4b check.sh
	@./check.sh

clean:
	@rm -f lab4b log.txt lab4b-704766877.tar.gz

dist:
	@tar -czf lab4b-704766877.tar.gz lab4b.c Makefile README check.sh

