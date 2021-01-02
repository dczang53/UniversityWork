#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include "SortedList.h"
#include <string.h>

int opt_yield;

SortedList_t *base_list;

pthread_mutex_t m_lock;
int s_lock = 0;

void SortedList_insert_m(SortedList_t *list, SortedListElement_t *element)
{
  pthread_mutex_lock(&m_lock);
  if(list == NULL || element == NULL || list->key != NULL)
    {
      pthread_mutex_unlock(&m_lock);
      return;
    }
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      if(strcmp(current->key, element->key) >= 0)
	break;
      current = current->next;
    }
  SortedListElement_t *previous = current->prev;
  if(opt_yield & INSERT_YIELD)
    sched_yield();
  element->next = current;
  element->prev = previous;
  current->prev = element;
  previous->next = element;
  pthread_mutex_unlock(&m_lock);
}

void SortedList_insert_s(SortedList_t *list, SortedListElement_t *element)
{
  while(__sync_lock_test_and_set(&s_lock, 1))
    {}
  if(list == NULL || element == NULL || list->key != NULL)
    {
      __sync_lock_release(&s_lock);
      return;
    }
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      if(strcmp(current->key, element->key) >= 0)
	break;
      current = current->next;
    }
  SortedListElement_t *previous = current->prev;
  if(opt_yield & INSERT_YIELD)
    sched_yield();
  element->next = current;
  element->prev = previous;
  current->prev = element;
  previous->next = element;
  __sync_lock_release(&s_lock);
}



int SortedList_delete_m(SortedListElement_t *element)
{
  pthread_mutex_lock(&m_lock);
  if(element == NULL || element->next->prev != element || element->prev->next != element)
    {
      pthread_mutex_unlock(&m_lock);
      return 1;
    }
  if(opt_yield & DELETE_YIELD)
    sched_yield();
  element->next->prev = element->prev;
  element->prev->next = element->next;
  pthread_mutex_unlock(&m_lock);
  return 0;
}

int SortedList_delete_s(SortedListElement_t *element)
{
  while(__sync_lock_test_and_set(&s_lock, 1))
    {}
  if(element == NULL || element->next->prev != element || element->prev->next != element)
    {
      __sync_lock_release(&s_lock);
      return 1;
    }
  if(opt_yield & DELETE_YIELD)
    sched_yield();
  element->next->prev = element->prev;
  element->prev->next = element->next;
  __sync_lock_release(&s_lock);
  return 0;
}



SortedListElement_t *SortedList_lookup_m(SortedList_t *list, const char *key)
{
  pthread_mutex_lock(&m_lock);
  if(list == NULL || list->key != NULL || key == NULL)
    {
      pthread_mutex_unlock(&m_lock);
      return NULL;
    }
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      if(strcmp(current->key, key) == 0)
	{
	  pthread_mutex_unlock(&m_lock);
	  return current;
	}
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      current = current->next;
    }
  pthread_mutex_unlock(&m_lock);
  return NULL;
}

SortedListElement_t *SortedList_lookup_s(SortedList_t *list, const char *key)
{
  while(__sync_lock_test_and_set(&s_lock, 1))
    {}
  if(list == NULL || key == NULL)
    {
      __sync_lock_release(&s_lock);
      return NULL;
    }
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      if(strcmp(current->key, key) == 0)
	{
	  __sync_lock_release(&s_lock);
	  return current;
	}
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      current = current->next;
    }
  __sync_lock_release(&s_lock);
  return NULL;
}



int SortedList_length_m(SortedList_t *list)
{
  pthread_mutex_lock(&m_lock);
  if(list == NULL || list->key != NULL)
    {
      pthread_mutex_unlock(&m_lock);
      return 0;
    }
  int count = 0;
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      count++;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      current = current->next;
    }
  pthread_mutex_unlock(&m_lock);
  return count;
}

int SortedList_length_s(SortedList_t *list)
{
  while(__sync_lock_test_and_set(&s_lock, 1))
    {}
  if(list == NULL || list->key != NULL)
    {
      __sync_lock_release(&s_lock);
      return 0;
    }
  int count = 0;
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      count++;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      current = current->next;
    }
  __sync_lock_release(&s_lock);
  return count;
}


int threads = 1;
int iterations = 1;
struct run_functions
{
  void (*insert)(SortedList_t *, SortedListElement_t *);
  int (*delete)(SortedListElement_t *);
  SortedListElement_t* (*lookup)(SortedList_t *, const char *);
  int (*length)(SortedList_t *);
};
struct run_functions run;
void *driver_function(void *arg)
{
  int iter = iterations;
  struct run_functions func = run;
  SortedList_t *base = base_list;
  SortedListElement_t *elements = (SortedListElement_t *) arg;
  int cnt;
  for(cnt = 0; cnt < iter; cnt++)
    {
      func.insert(base, &elements[cnt]);
    }
  func.length(base);
  SortedListElement_t *lookedup;
  for(cnt = 0; cnt < iter; cnt++)
    {
      if((lookedup = func.lookup(base, elements[cnt].key)) == NULL)
	{
	  fprintf(stderr, "Error: lookup failed\n");
	  exit(2);
	}
      if(func.delete(&elements[cnt]))
	{
	  fprintf(stderr, "Error: invalid delete\n");
	  exit(2);
	}
    }
  return NULL;
}



int main(int argc, char *argv[])
{
  char *tag_list = "list";
  char *tag_1 = "-none";
  char *tag_2 = "-s";
  char *tag_3 = "-m";
  char tag_yield[6] = "-none";
  char *tag_sync = tag_1;
  struct option long_options[] =
    {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", required_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
  opt_yield = 0;
  int state;
  char *yield_option;
  char *sync_option;
  run = (struct run_functions) { SortedList_insert, SortedList_delete, SortedList_lookup, SortedList_length };
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
	  yield_option = optarg;
	  int cnt = 0;
	  int strcnt = strlen(yield_option);
	  if(strcnt > 0 && strcnt <= 3)
	    {
	      while(cnt < strcnt)
		{
		  switch(yield_option[cnt])
		    {
		    case 'i':
		      opt_yield |= INSERT_YIELD;
		      break;
		    case 'd':
		      opt_yield |= DELETE_YIELD;
		      break;
		    case 'l':
		      opt_yield |= LOOKUP_YIELD;
		      break;
		    default:
		      fprintf(stderr, "Error: invalid yield option (should be i, d, or l)\n");
		      exit(1);
		    }
		  cnt++;
		}
	    }
	  else
	    {
	      fprintf(stderr, "Error: invalid number of yield options\n");
	      exit(1);
	    }
	  break;
	case 's':
	  sync_option = optarg;
	  if(strlen(sync_option) == 1)
	    {
	      switch(*sync_option)
		{
		case 's':
		  run = (struct run_functions) { SortedList_insert_s, SortedList_delete_s, SortedList_lookup_s, SortedList_length_s };
		  tag_sync = tag_2;
		  break;
		case 'm':
		  run = (struct run_functions) { SortedList_insert_m, SortedList_delete_m, SortedList_lookup_m, SortedList_length_m };
		  tag_sync = tag_3;
		  break;
		default:
		  fprintf(stderr, "Error: invalid sync option (should be s or m)\n");
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
  if(opt_yield)
    {
      char i[] = "i";
      char d[] = "d";
      char l[] = "l";
      memset(tag_yield + 1, '\0', 5 * sizeof(char));
      if(opt_yield & INSERT_YIELD)
	strcat(tag_yield, i);
      if(opt_yield & DELETE_YIELD)
	strcat(tag_yield, d);
      if(opt_yield & LOOKUP_YIELD)
	strcat(tag_yield, l);
    }

  base_list = malloc(sizeof(SortedList_t));
  base_list->next = base_list;
  base_list->prev = base_list;
  base_list->key = NULL;

  //random string generation idea from
  //https://www.codeproject.com/Questions/640193/Random-string-in-language-C
  //https://ubuntuforums.org/showthread.php?t=1979310
  //https://www.geeksforgeeks.org/rand-and-srand-in-ccpp/
  //https://stackoverflow.com/questions/28485801/what-characters-are-legal-to-use-in-string-literals
  srand((unsigned int) time(0));
  char *rand_char = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890!@#$%^&*()-=_+[]{}|;:,.<>/?"; //89
  int thread_cnt = threads;
  int iteration_cnt = iterations;
  SortedListElement_t **keys = malloc(threads * sizeof(SortedListElement_t *));
  int thread_current = 0;
  int iteration_current = 0;
  int key_current = 0;
  char *new_key;
  for(; thread_current < thread_cnt; thread_current++)
    {
      keys[thread_current] = malloc(iterations * sizeof(SortedListElement_t));
      for(;iteration_current < iteration_cnt; iteration_current++)
	{
	  new_key = malloc(256 * sizeof(char));
	  for(; key_current < 255; key_current++)
	    {
	      new_key[key_current] = rand_char[rand() % 90];
	    }
	  new_key[key_current] = '\0';
	  key_current = 0;
	  keys[thread_current][iteration_current].key = new_key;
	}
      iteration_current = 0;
    }

  struct timespec start;
  struct timespec end;
  pthread_t *pthreads = malloc(sizeof(pthread_t) * threads);
  int count1 = 0;
  int count2 = 0;
  clock_gettime(CLOCK_MONOTONIC, &start);
  for(; count1 < thread_cnt; count1++)
    {
      if(pthread_create(&pthreads[count1], NULL, driver_function, (void *) keys[count1]))
	{
	  fprintf(stderr, "Error: thread creation failed\n");
	  free(pthreads);
	  int rm_thread = 0;
	  int rm_element = 0;
	  for(;rm_thread < thread_cnt; rm_thread++)
	    {
	      for(;rm_element < iteration_cnt; rm_element++)
		{
		  free((void *) keys[rm_thread][rm_element].key);
		}
	      rm_element = 0;
	      free(keys[rm_thread]);
	    }
	  free(base_list);
	  exit(2);
	}
    }
  for(; count2 < thread_cnt; count2++)
    {
      if(pthread_join(pthreads[count2], NULL))
	{
	  fprintf(stderr, "Error: thread retrieval failed\n");
	  free(pthreads);
	  int rm_thread = 0;
	  int rm_element = 0;
	  for(;rm_thread < thread_cnt; rm_thread++)
	    {
	      for(;rm_element < iteration_cnt; rm_element++)
		{
		  free((void *) keys[rm_thread][rm_element].key);
		}
	      rm_element = 0;
	      free(keys[rm_thread]);
	    }
	  free(base_list);
	  exit(2);
	}
    }
  clock_gettime(CLOCK_MONOTONIC, &end);
  if(SortedList_length(base_list))
    {
      fprintf(stderr, "Error: Final length not 0\n");
      exit(2);
    }
  long long elapsed = 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
  int operations = threads * iterations * 3;
  long long tpo = elapsed / operations;
  printf("%s%s%s,%d,%d,1,%d,%lld,%lld\n", tag_list, tag_yield, tag_sync, threads, iterations, operations, elapsed, tpo);
  free(pthreads);
  int rm_thread = 0;
  int rm_element = 0;
  for(;rm_thread < thread_cnt; rm_thread++)
    {
      for(;rm_element < iteration_cnt; rm_element++)
	{
	  free((void *) keys[rm_thread][rm_element].key);
	}
      rm_element = 0;
      free(keys[rm_thread]);
    }
  free(base_list);
  exit(0);
}
