#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

long long counter = 0;
int opt_yield;
void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

pthread_mutex_t m_lock;
void add_m(long long *pointer, long long value) {
  pthread_mutex_lock(&m_lock);
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
  pthread_mutex_unlock(&m_lock);
}

int s_lock = 0;
void add_s(long long *pointer, long long value) {
  while(__sync_lock_test_and_set(&s_lock, 1))
    {}
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
  __sync_lock_release(&s_lock);
}

void add_c(long long *pointer, long long value) {
  long long previous;
  long long sum;
  while(1)
    {
      previous = *pointer;
      sum = previous + value;
      if (opt_yield)
	sched_yield();
      if((__sync_val_compare_and_swap(pointer, previous, sum)) == previous)
	break;
    }
}

int iterations = 1;
void (*function)(long long *, long long);
void *driver_function()
{
  int iter = iterations;
  void (*run)(long long *, long long) = function;
  int cnt;
  for(cnt = 0; cnt < iter; cnt++)
    {
      run(&counter, 1);
    }
  for(cnt = 0; cnt < iter; cnt++)
    {
      run(&counter, -1);
    }
  return NULL;
}

int main(int argc, char *argv[])
{
  char *tag_add = "add";
  char *tag_no_yield = "";
  char *tag_yield = "-yield";
  char *tag_1 = "-none";
  char *tag_2 = "-m";
  char *tag_3 = "-s";
  char *tag_4 = "-c";
  char *tag_a = tag_no_yield;
  char *tag_b = tag_1;
  struct option long_options[] =
    {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", no_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
  int threads = 1;
  opt_yield = 0;
  int state;
  char *sync_option;
  function = add;
  if(pthread_mutex_init(&m_lock, NULL))
    {
      fprintf(stderr, "Error: mutex initialization failed\n");
      exit(2);
    }
  while((state = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
      switch(state)
	{
	case 't':
	  threads = atoi(optarg);
	  break;
	case 'i':
	  iterations = atoi(optarg);
	  break;
	case 'y':
	  opt_yield = 1;
	  tag_a = tag_yield;
	  break;
	case 's':
	  sync_option = optarg;
	  if(strlen(sync_option) == 1)
	    {
	      switch(*sync_option)
		{
		case 'm':
		  function = add_m;
		  tag_b = tag_2;
		  break;
		case 's':
		  function = add_s;
		  tag_b = tag_3;
		  break;
		case 'c':
		  function = add_c;
		  tag_b = tag_4;
		  break;
		default:
		  fprintf(stderr, "Error: invalid sync option (should be m, s, or c)\n");
		  exit(1);
		}
	    }
	  else
	    {
	      fprintf(stderr, "Error: invalid sync option (should only be one char)\n");
	      exit(1);
	    }
	  break;
	default:
	  fprintf(stderr, "Invalid option\n");
	  exit(1);
	}
    }
  if(threads < 0)
    {
      fprintf(stderr, "Error: invalid number of threads\n");
      exit(1);
    }
  if(iterations < 0)
    {
      fprintf(stderr, "Error: invalid number of iterations\n");
      exit(1);
    }

  struct timespec start;
  struct timespec end;
  pthread_t *pthreads = malloc(sizeof(pthread_t) * threads);
  int count1 = 0;
  int count2 = 0;
  clock_gettime(CLOCK_MONOTONIC, &start);
  for(; count1 < threads; count1++)
    {
      if(pthread_create(&pthreads[count1], NULL, driver_function, NULL))
	{
	  fprintf(stderr, "Error: thread creation failed\n");
	  free(pthreads);
	  exit(2);
	}
    }
  for(; count2 < threads; count2++)
    {
      if(pthread_join(pthreads[count2], NULL))
	{
	  fprintf(stderr, "Error: thread retrieval failed\n");
	  free(pthreads);
	  exit(2);
	}
    }
  clock_gettime(CLOCK_MONOTONIC, &end);
  long long elapsed = 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
  int operations = threads * iterations * 2;
  long long tpo = elapsed / operations;
  printf("%s%s%s,%d,%d,%d,%lld,%lld,%lld\n", tag_add, tag_a, tag_b, threads, iterations, operations, elapsed, tpo, counter);
  free(pthreads);
  exit(0);
}
