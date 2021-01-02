CC = gcc
CFLAGS = -Wall -Wextra -lpthread
add_iterations = 10 20 40 80 100 1000 10000 100000
list_iterations_a = 10 100 1000 10000 100000
list_iterations_b = 1 2 4 8 12  16 24
threads = 1, 2, 4, 8, 12, 16, 24
add_threads = 2 8

.PHONY: build tests graphs dist clean

build:
	@$(CC) $(CFLAGS) -o lab2_add lab2_add.c
	@$(CC) $(CFLAGS) -o lab2_list SortedList.c lab2_list.c

tests: lab2_add lab2_list
	@rm -f *.csv
	@$(foreach x,2 4 8 12,$(foreach y,10 20 40 80 100 1000 10000 100000,./lab2_add --threads=$x --iterations=$y --yield >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield >> lab2_add.csv;))

	@$(foreach x,2 8,$(foreach y,100 1000 10000 100000,./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield >> lab2_add.csv;))

	@$(foreach x,1,$(foreach y,100 1000 10000 100000,./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;))

	@$(foreach y,10000,$(foreach x,2 4 8 12,./lab2_add --threads=$x --iterations=$y --yield --sync=m >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=m >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=m >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=c >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=c >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=c >> lab2_add.csv;))
	@$(foreach y,1000,$(foreach x,2 4 8 12,./lab2_add --threads=$x --iterations=$y --yield --sync=s >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=s >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --yield --sync=s >> lab2_add.csv;))

	@$(foreach y,10000,$(foreach x,1 2 4 8 12,./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=m >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=m >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=m >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=s >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=s >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=s >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=c >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=c >> lab2_add.csv;./lab2_add --threads=$x --iterations=$y --sync=c >> lab2_add.csv;))

	@-$(foreach x,1,$(foreach y,10 100 1000 10000 20000,./lab2_list --threads=$x --iterations=$y >> lab2_list.csv;))

	@-$(foreach x,2 4 8 12,$(foreach y,1 10 100 1000,./lab2_list --threads=$x --iterations=$y >> lab2_list.csv;))
	@-$(foreach x,2 4 8 12,$(foreach y,1 2 4 8 16 32, $(foreach z,i d il dl,./lab2_list --threads=$x --iterations=$y --yield=$z >> lab2_list.csv;)))

	@-$(foreach w,m s,$(foreach x,2 4 8 12,$(foreach y,1 2 4 8 16 32, $(foreach z,i d il dl,./lab2_list --sync=$w  --threads=$x --iterations=$y --yield=$z >> lab2_list.csv;))))

	@-$(foreach w,m s,$(foreach x,1 2 4 8 12 16 24,$(foreach y,1000,./lab2_list --sync=$w  --threads=$x --iterations=$y >> lab2_list.csv;)))

graphs: lab2_add.csv lab2_list.csv
	@gnuplot lab2_add.gp
	@gnuplot lab2_list.gp

dist: build tests graphs
	@tar -czf lab2a-704766877.tar.gz lab2_add.c SortedList.h SortedList.c lab2_list.c Makefile lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png README lab2_add.gp lab2_list.gp

clean:
	@rm -f lab2_add lab2_list lab2a-704766877.tar.gz
