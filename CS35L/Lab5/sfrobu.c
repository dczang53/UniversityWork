#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

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

int frobcmp2(char const *a, char const *b)
{
	while (*a == *b)
	{
		if (*a == ' ')
			return 0;
		a++;
		b++;
	}
	signed char c = ((*a)^42);
	signed char d = ((*b)^42);
	if (*a == ' ')
		return -1;
	else if (*b == ' ')
		return 1;
	else if (c > 0 && d > 0)
		if (toupper(c) != toupper(d))
			return toupper(c) > toupper(d);
		else
			return 0;
	else
		if (c != d)
			return c > d;
		else
			return 0;
}

int fcmp2(const void *a, const void *b)
{
	const char *c = *(char**)a;
	const char *d = *(char**)b;
	return frobcmp2(c, d);
}

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		fprintf(stderr, "Must have at most one option");
		exit(1);
	}
	int (*fn)(const void*, const void*);
	if (argc == 2)
	{
		char *test = "-f";
		if (!strcmp(argv[1], test))
		{
			fn = &fcmp2;
		}
		else
		{
			fprintf(stderr, "Invalid option");
			exit(1);
		}
	}
	else
		fn = &fcmp;
	char *all, **linebuf, curchar[1], *new;
	int asize, cursize, linenum;
	bool empty;
	ssize_t num;
	cursize = 0;
	linenum = 0;
	struct stat file;
	if (fstat(STDIN_FILENO, &file) < 0)
	{
		fprintf(stderr, "fstat() error (Error: %d)\n", errno);
		exit(1);
	}
	if (S_ISREG(file.st_mode))
		asize = file.st_size + 1;
	else
		asize = 16;
	all = (char*) malloc(sizeof(char) * asize);
	if (all == NULL)
	{
		fprintf(stderr, "Failed to allocate memory (Error: %d)\n", errno);
		exit(1);
	}

	num = read(STDIN_FILENO, curchar, 1);
	empty = true;
	while (num > 0)
	{
		if (empty && curchar[0] == ' ')
		{
			num = read(STDIN_FILENO, curchar, 1);
			continue;
		}
		if (curchar[0] == ' ')
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
			all[cursize] = curchar[0];
			cursize++;
			num = read(STDIN_FILENO, curchar, 1);
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
		all[cursize] = curchar[0];
		cursize++;
		num = read(STDIN_FILENO, curchar, 1);
	}
	if (num < 0)
	{
		free(all);
		fprintf(stderr, "IO Error (Error: %d)\n", errno);
   		exit(1);
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

	qsort(linebuf, linenum, sizeof(char*), fn);

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









