CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: default check clean dist

default:
	@$(CC) $(CFLAGS) -o lab0 lab0.c

check: lab0
	@echo -e "qwertyuiop\nasdfghjkl\nzxcvbnm\n" > expected.txt
	@echo -e "TESTING: STDIN to STDOUT"
	@-cat expected.txt | ./lab0 > output1.txt; if [[ $$? -ne 0 ]]; then echo "   Fail: (STDIN to STDOUT) Exit status not 0"; else echo "   Exited properly (0)"; fi
	@if ! cmp -s "expected.txt" "output1.txt"; then echo "   Fail: (STDIN to STDOUT) Improper copy"; else echo "   No problems w/ copying"; fi
	@echo -e "TESTING: FAULTY ARG"
	@-cat expected.txt | ./lab0 --fault 2> /dev/null; if [[ $$? -ne 1 ]]; then echo "   Fail: (FAULTY ARG) Exit status not 1"; else echo "   Exited properly (1)"; fi
	@echo -e "TESTING: --input to STDOUT"
	@-./lab0 --input=expected.txt > output2.txt; if [[ $$? -ne 0 ]]; then echo "   Fail: (--input to STDOUT) Exit status not 0"; else echo "   Exited properly (0)"; fi
	@if ! cmp -s "expected.txt" "output2.txt"; then echo "   Fail: (--input to STDOUT) Improper copy"; else echo "   No problems w/ copying"; fi
	@echo -e "TESTING: STDIN to --output"
	@-cat expected.txt | ./lab0 --output=output3.txt; if [[ $$? -ne 0 ]]; then echo "   Fail: (STDIN to --output) Exit status not 0"; else echo "   Exited properly (0)"; fi
	@if ! cmp -s "expected.txt" "output3.txt"; then echo "   Fail: (STDIN to --output) Improper copy"; else echo "   No problems w/ copying"; fi
	@echo -e "TESTING: --input to --output"
	@-./lab0 --input=expected.txt --output=output4.txt; if [[ $$? -ne 0 ]]; then echo "   Fail: (--input to --output) Exit status not 0"; else echo "   Exited properly (0)"; fi
	@if ! cmp -s "expected.txt" "output4.txt"; then echo "   Fail: (--input to --output) Improper copy"; else echo "   No problems w/ copying"; fi
	@echo -e "TESTING: FAULTY --input"
	@-./lab0 --input --output=output5.txt 2> /dev/null; if [[ $$? -ne 2 ]]; then echo "   Fail: (FAULTY --input) Exit status not 2"; else echo "   Exited properly (2)"; fi
	@echo -e "TESTING: FAULTY --output"
	@touch output6.txt
	@chmod 433 output6.txt
	@-./lab0 --input=expected.txt --output=output6.txt 2> /dev/null; if [[ $$? -ne 3 ]]; then echo "   Fail: (FAULTY --output) Exit status not 3"; else echo "   Exited properly (3)"; fi
	@echo -e "TESTING: --segfault (Should output Segmentation fault)"
	@-./lab0 --input=expected.txt --segfault 2> /dev/null; if [[ $$? -ne 139 ]]; then echo "   Fail: (--segfault w/ --input) Exit status not 139 (SIGSEGV)"; else echo "   Exited properly (139)"; fi
	@echo -e "TESTING: --catch w/ --segfault"
	@-./lab0 --input=expected.txt --segfault --catch 2> /dev/null; if [[ $$? -ne 4 ]]; then echo "   Fail: (--catch w/ --segfault) Exit status not 4"; else echo "   Exited properly (4)"; fi
	@rm -f *.txt
	@echo -e "*All test cases done.*"

clean:
	@rm -f lab0 lab0-704766877.tar.gz

dist:
	@tar -czf lab0-704766877.tar.gz lab0.c Makefile backtrace.png breakpoint.png README
