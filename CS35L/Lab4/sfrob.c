#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

//In style of memcmp
int frobcmp(char const *a, char const *b)
{
	while (*a == *b)
	{
		if (*a == ' ')
			return 0;
		a++;
		b++;
	}
	if (*a == ' ')
		return -1;
	else if (*b == ' ' || ((*a)^42) > ((*b)^42))
		return 1;
	else
		return -1;
}


int fcmp(const void *a, const void *b)
{
	const char *c = *(char**)a;
	const char *d = *(char**)b;
	return frobcmp(c, d);
}


int main()
{
	char *all, **linebuf, curchar, *new;
	int asize, cursize, linenum;
	bool empty;
	asize = 16;
	cursize = 0;
	linenum = 0;
	all = (char*) malloc(sizeof(char) * asize);
	if (all == NULL)
	{
		fprintf(stderr, "Failed to allocate memory (Error: %d)\n", errno);
		exit(1);
	}
	curchar = getchar();
	if (ferror(stdin))
	{
		free(all);
		fprintf(stderr, "IO Error (Error: %d)\n", errno);
   		exit(1);
	}
	empty = true;
	while (curchar != EOF)
	{
		if (empty && curchar == ' ')
		{
			curchar = getchar();
			if (ferror(stdin) && curchar != EOF)
			{
				free(all);
				fprintf(stderr, "IO Error (Error: %d)\n", errno);
	   			exit(1);
			}
			continue;
		}
		if (curchar == ' ')
		{
			linenum++;
			if (cursize == asize)
			{
				asize *= 2;
				new = (char*) realloc(all, sizeof(char) * asize);
				if (new == NULL)
				{
					free(all);
					fprintf(stderr, "Failed to allocate memory (Error: %d)\n", errno);
					exit(1);
				}
				all = new;
			}
			all[cursize] = curchar;
			cursize++;
			curchar = getchar();
			if (ferror(stdin) && curchar != EOF)
			{
				free(all);
				fprintf(stderr, "IO Error (Error: %d)\n", errno);
	   			exit(1);
			}
			empty = true;
			continue;
		}
		empty = false;
		if (cursize == asize)
		{
			asize *= 2;
			new = (char*) realloc(all, sizeof(char) * asize);
			if (new == NULL)
			{
				free(all);
				fprintf(stderr, "Failed to allocate memory (Error: %d)\n", errno);
				exit(1);
			}
			all = new;
		}
		all[cursize] = curchar;
		cursize++;
		curchar = getchar();
		if (ferror(stdin) && curchar != EOF)
		{
			free(all);
			fprintf(stderr, "IO Error (Error: %d)\n", errno);
			exit(1);
		}
	}

	if (!empty)
	{
		linenum++;
		if (cursize == asize)
		{
			asize *= 2;
			new = (char*) realloc(all, sizeof(char) * asize);
			if (new == NULL)
			{
				free(all);
				fprintf(stderr, "Failed to allocate memory (Error: %d)\n", errno);
				exit(1);
			}
			all = new;
		}
		all[cursize] = ' ';
		cursize++;
	}

	linebuf = (char**) malloc(sizeof(char*) * linenum);
	if (linebuf == NULL)
	{
		free(all);
		fprintf(stderr, "Failed to allocate memory (Error: %d)\n", errno);
		exit(1);
	}
	int x = 1;
	char *n = all;
	if (linenum > 0)
		linebuf[0] = n;
	for(; x < linenum; n++)
	{
		if (*n == ' ')
		{
			linebuf[x] = ++n;
			x++;
		}
	}

	qsort(linebuf, linenum, sizeof(char*), fcmp);

	int z = 0;
	int a = 0;
	for (; z < linenum; z++)
	{
		while (linebuf[z][a] != ' ')
		{
			putchar(linebuf[z][a]);
			a++;
		}
		putchar(' ');
		a = 0;
	}

	free(all);
	free(linebuf);
	return 0;
}









