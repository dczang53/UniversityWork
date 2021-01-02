#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>

//https://www.gnu.org/software/libc/manual/html_node/Terminal-Modes.html
//http://man7.org/linux/man-pages/man3/termios.3.html
//http://pubs.opengroup.org/onlinepubs/009695399/functions/tcgetattr.html
//http://pubs.opengroup.org/onlinepubs/9699919799/functions/tcsetattr.html
//https://www.gnu.org/software/libc/manual/html_node/Canonical-or-Not.html#Canonical-or-Not
//https://www.gnu.org/software/libc/manual/html_node/Noncanonical-Input.html#Noncanonical-Input

//http://www.unixwiz.net/techtips/termios-vmin-vtime.html

//http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

//https://stackoverflow.com/questions/20154157/termios-vmin-vtime-and-blocking-non-blocking-read-operations
//http://www.cmrr.umn.edu/~strupp/serial.html#2_5_4

struct termios stdin_init;

void handler(int signum)
{
  fprintf(stderr, "\r\nError: (SIGPIPE) error number %d\r\n", signum);
  tcsetattr(0, TCSANOW, &stdin_init);
  _exit(0);
}

int main(int argc, char *argv[])
{
  struct option long_options[] = 
    {
      {"shell", no_argument, 0, 's'},
      {0, 0, 0, 0}
    };
  int shell_flag = 0;
  int state;
  while((state = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
      switch(state)
	{
	case 's':
	  shell_flag = 1;
	  break;
	default:
	  fprintf(stderr, "Invalid option\n");
	  _exit(1);
	}
    }
  if(tcgetattr(0, &stdin_init))
    {
      fprintf(stderr, "Error: tcgetattr failed (%s)\n", strerror(errno));
      _exit(1);
    }
  struct termios stdin_noncanon = stdin_init;
  stdin_noncanon.c_iflag = ISTRIP;
  stdin_noncanon.c_oflag = 0;
  stdin_noncanon.c_lflag = 0;
  stdin_noncanon.c_cc[VMIN] = 1;
  stdin_noncanon.c_cc[VTIME] = 0;
  if(tcsetattr(0, TCSANOW, &stdin_noncanon))
    {
      fprintf(stderr, "Error: tcsetattr failed (%s)\n", strerror(errno));
      _exit(1);
    }
  char buffer[10];
  char w_buffer[20];
  int num_read;
  if(!shell_flag)
    {
      while((num_read = read(0, buffer, 10)) >= 0)
	{
	  int w_current = 0;
	  int b_current = 0;
	  int flag = 0;
	  for(; b_current < num_read; b_current++)
	    {
	      if(buffer[b_current] == '\r' || buffer[b_current] == '\n')
		{
		  w_buffer[w_current] = '\r';
		  w_current++;
		  w_buffer[w_current] = '\n';
		  w_current++;
		}
	      else if(buffer[b_current] == 0x04)
		{
		  flag = 1;
		  break;
		}
	      else
		{
		  w_buffer[w_current] = buffer[b_current];
		  w_current++;
		}
	    }
	  if(w_current && ((write(1, w_buffer, w_current)) < 0))
	    {
	      fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
	      tcsetattr(0, TCSANOW, &stdin_init);
	      _exit(1);
	    }
	  if(flag)
	    break;
	}
      if(num_read < 0)
	{
	  fprintf(stderr, "Error: read failed (%s)\r\n", strerror(errno));
	  tcsetattr(0, TCSANOW, &stdin_init);
	  _exit(1);
	}
    }
  else
    {
      int pipeA[2];
      int pipeB[2];
      int pid;
      if(pipe(pipeA) < 0)
	{
	  fprintf(stderr, "Error creating pipe\r\n");
	  tcsetattr(0, TCSANOW, &stdin_init);
	  _exit(1);
	}
      if(pipe(pipeB) < 0)
	{
	  fprintf(stderr, "Error creating pipe\r\n");
	  tcsetattr(0, TCSANOW, &stdin_init);
	  _exit(1);
	}
      pid = fork();
      if(pid < 0)
	{
	  fprintf(stderr, "Error with fork\r\n");
	  tcsetattr(0, TCSANOW, &stdin_init);
	  _exit(1);
	}
      else if(pid > 0)   //PARENT
	{
	  signal(SIGPIPE, handler);
	  close(pipeA[0]);   //https://github.com/raoulmillais/linux-system-programming/blob/master/src/poll-example.c
	  close(pipeB[1]);
	  struct pollfd rfds[2];
	  rfds[0].fd = 0;
	  rfds[0].events = POLLIN | POLLHUP | POLLERR;
	  rfds[1].fd = pipeB[0];
	  rfds[1].events = POLLIN | POLLHUP | POLLERR;
	  int status;
	  int write_closed = 0;
	  char shell_input[10];
	  char shell_return[256];
	  char shell_print[512];
	  int flag = 0;
	  int c_cflag = 0;
	  while((status = poll(rfds, 2, 0)) >= 0)
	    {
	      if(c_cflag)
		{
		  char fault[1] = "a";
		  write(pipeA[1], fault, 1);
		}
	      //READ SEGMENT
	      if(rfds[0].revents & POLLIN)
		{
		  if((num_read = read(0, buffer, 10)) < 0)
		    {
		      fprintf(stderr, "Error: read error (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		  int w_current = 0;
		  int shell = 0;
		  int b_current = 0;
		  for(; b_current < num_read; b_current++)
		    {
		      if(buffer[b_current] == '\r' || buffer[b_current] == '\n')
			{
			  w_buffer[w_current] = '\r';
			  w_current++;
			  w_buffer[w_current] = '\n';
			  w_current++;
			  shell_input[shell] = '\n';
			  shell++;
			}
		      else if(buffer[b_current] == 0x03)
			{
			  kill(pid, SIGINT);
			  w_buffer[w_current] = '^';
			  w_current++;
			  w_buffer[w_current] = 'C';
			  w_current++;
			  c_cflag = 1;
			  break;
			}
		      else if(buffer[b_current] == 0x04)
			{
			  flag = 1;
			  w_buffer[w_current] = '^';
			  w_current++;
			  w_buffer[w_current] = 'D';
			  w_current++;
			  break;
			}
		      else
			{
			  w_buffer[w_current] = buffer[b_current];
			  w_current++;
			  shell_input[shell] = buffer[b_current];
			  shell++;
			}
		    }
		  if(!write_closed && shell && ((write(pipeA[1], shell_input, shell)) < 0))
		    {
		      fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		  if(w_current && ((write(1, w_buffer, w_current)) < 0))
		    {
		      fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		  if(flag)
		    {
		      close(pipeA[1]);
		      write_closed = 1;   //Just in case
		    }
		}

	      //SHELL READ SEGMENT
	      if(rfds[1].revents & POLLIN)
		{
		  if((num_read = read(pipeB[0], shell_return, 256)) < 0)
		    {
		      fprintf(stderr, "Error: read error (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		  int w_current = 0;
		  int b_current = 0;
		  for(; b_current < num_read; b_current++)
		    {
		      if(shell_return[b_current] == '\n')
			{
			  shell_print[w_current] = '\r';
			  w_current++;
			  shell_print[w_current] = '\n';
			  w_current++;
			}
		      else
			{
			  shell_print[w_current] = shell_return[b_current];
			  w_current++;
			}
		    }
		  if(w_current && ((write(1, shell_print, w_current)) < 0))
		    {
		      fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		}

	      //ERROR SEGMENT
	      if((rfds[1].revents & (POLLHUP | POLLERR)) && write_closed)
		{
		  close(pipeB[0]);
		  break;
		}    
	    }
	  if(status < 0)
	    {
	      fprintf(stderr, "Error: poll error (%s)\r\n", strerror(errno));
	      tcsetattr(0, TCSANOW, &stdin_init);
	      _exit(1);
	    }
	  int end_status;
	  if((waitpid(pid, &end_status, 0)) < 0)
	    {
	      fprintf(stderr, "Error: waitpid failed\r\n");
	      tcsetattr(0, TCSANOW, &stdin_init);
	      _exit(1);
	    }
	  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n", WTERMSIG(end_status), WEXITSTATUS(end_status));
	}
      else   //CHILD
	{
	  close(pipeA[1]);
	  close(pipeB[0]);
	  dup2(pipeA[0], 0);
	  dup2(pipeB[1], 1);
	  dup2(pipeB[1], 2);
	  close(pipeA[0]);
	  close(pipeB[1]);
	  char name[] = "/bin/bash";
	  char* const *argv = NULL;
	  if(execvp(name, argv))
	    {
	      fprintf(stderr, "Error: exec failed\r\n");
	      _exit(1);
	    }
	}
    }
  tcsetattr(0, TCSANOW, &stdin_init);
  _exit(0);
}
