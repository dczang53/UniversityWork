#include "SortedList.h"
#include <string.h>
#include <sched.h>

int opt_yield;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  if(list == NULL || element == NULL || list->key != NULL)
    return;
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
}

int SortedList_delete(SortedListElement_t *element)
{
  if(element == NULL || element->next->prev != element || element->prev->next != element)
    return 1;
  if(opt_yield & DELETE_YIELD)
    sched_yield();
  element->next->prev = element->prev;
  element->prev->next = element->next;
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  if(list == NULL || key == NULL)
    return NULL;
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      if(strcmp(current->key, key) == 0)
	return current;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      current = current->next;
    }
  return NULL;
}

int SortedList_length(SortedList_t *list)
{
  if(list == NULL || list->key != NULL)
    return 0;
  int count = 0;
  SortedListElement_t *current = list->next;
  while(current->key != NULL)
    {
      count++;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      current = current->next;
    }
  return count;
}
