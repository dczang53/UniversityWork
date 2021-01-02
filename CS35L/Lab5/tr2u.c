#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

	char current[1];
	ssize_t num;
	num = read(STDIN_FILENO, current, 1);
	while(num > 0)
	{
		for(x = 0; x < a; x++)
		{
			if(current[0] == from[x])
			{
				current[0] = to[x];
				break;
			}
		}
		write(STDOUT_FILENO, current, 1);
		num = read(STDIN_FILENO, current, 1);
	}
	if(num < 0)
	{
		fprintf(stderr, "Read error");
		exit(1);
	}

	return 0;
}

















