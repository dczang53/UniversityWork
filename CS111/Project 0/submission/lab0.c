#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>   //for error reports

void segfault()
{
  char *fault = NULL;
  *fault = 'A';
}

void handler(int signum)   //https://stackoverflow.com/questions/8833394/can-exit-fail-to-terminate-process
{
  fprintf(stderr, "Segmentation fault caught; Errno: %d\n", signum);
  _exit(4);
}

int main(int argc, char *argv[])
{
  struct option long_options[] =
    {
      {"input", required_argument, 0, 'a'},
      {"output", required_argument, 0, 'b'},
      {"segfault", no_argument, 0, 'c'},
      {"catch", no_argument, 0, 'd'},
      {0, 0, 0, 0}
    };
  int replacement;
  int segfault_flag = 0;
  int handler_flag = 0;
  int input_flag = 0;
  char *input_file;
  int output_flag = 0;
  char *output_file;
  int state;
  while((state = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
    switch(state)
      {
      //https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt
      //optarg can be assigned, as it points to the argument in the argv array (according to the spec)
      case 'a':
        input_flag = 1;
	input_file = optarg;
       	break;
      case 'b':
        output_flag = 1;
	output_file = optarg;
	break;
      case 'c':
	segfault_flag = 1;
	break;
      case 'd':
	handler_flag = 1;	
	break;
      default:             //http://man7.org/linux/man-pages/man2/signal.2.html
	fprintf(stderr, "Correct usage: lab0 [--options]\nDescription: read from standard input and write to standard output\nOptions:\n--input=filename ... use the specified file as standard input\n--output=filename ... create the specified file and use it as standard output\n--segfault ... force a segmentation fault\n--catch ... register a SIGSEGV handler that catches the segmentation fault\n");
	_exit(1);   //API will report the invalid option
      }
    }
  
  if(handler_flag)
    {
      signal(SIGSEGV, handler);
    }

  if(segfault_flag)   //cause a segmentation fault by writing to a null pointer
    {
      segfault();
    }

  if(input_flag)
    {
      if((replacement = open(input_file, O_RDONLY)) != -1)
	  {
	    close(0);
	    dup(replacement);
	    close(replacement);
	  }
	else
	  {
	    fprintf(stderr, "Error: (--input) Unable to open input file \'%s\'\n%s\n", input_file, strerror(errno));
	    _exit(2);
	  }
    }

  if(output_flag)
    {
      if((replacement = creat(output_file, 0733)) != -1)   //owner read, write, and execute; group and others can only read and execute
	  {
	    close(1);
	    dup(replacement);
	    close(replacement);
	  }
	else
	  {
	    fprintf(stderr, "Error: (--output) Unable to write to output file \'%s\'\n%s\n", output_file, strerror(errno));
	    _exit(3);
	  }
    }

  char buf[1];
  ssize_t count;
  while((count = read(0, buf, 1)) > 0)
    {
    write(1, buf, 1);
    }
  _exit(0);
}

//https://www.gnu.org/software/libc/manual/html_node/Getopt.html
//https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html#Getopt-Long-Options
