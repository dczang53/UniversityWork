#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Must have two operators\n");
		exit(1);
	}

	char *from = argv[1];
	char *to = argv[2];
	int a = strlen(from);
	int b = strlen(to);

	if(a != b)
	{
		fprintf(stderr, "Byte string lengths must be equal\n");
		exit(1);
	}

	int x;
	int y;
	for(x = 0; x < a; x++)
		for(y = x+1; y < a; y++)
			if(from[x] == from[y])
			{
				fprintf(stderr, "FIrst byte string cannot have duplicate bytes\n");
				exit(1);
			}

	int current = getchar();
	while(current != EOF)
	{
		for(x = 0; x < a; x++)
		{
			if(current == from[x])
			{
				current = to[x];
				break;
			}
		}
		putchar(current);
		current = getchar();
	}

	return 0;
}

















