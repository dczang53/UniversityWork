#include <stdio.h>
#include <stdlib.h>
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
#include <sys/socket.h>
#include <netinet/in.h>
#include "zlib.h"

//https://www.geeksforgeeks.org/socket-programming-cc/https://www.geeksforgeeks.org/socket-programming-cc/

struct termios stdin_init;
int client;

void handler(int signum)
{
  fprintf(stderr, "\r\nError: (SIGPIPE) error number %d\n", signum);
  close(client);
  _exit(0);
}

int main(int argc, char *argv[])
{
  struct option long_options[] = 
    {
      {"port", required_argument, 0, 'p'},
      {"compress", no_argument, 0, 'c'},
      {0, 0, 0, 0}
    };
  int compress_flag = 0;
  int port_num = 0;
  int state;
  while((state = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
      switch(state)
	{
	case 'p':
	  port_num = atoi(optarg);
	  break;
	case 'c':
	  compress_flag = 1;
	  break;  
	default:
	  fprintf(stderr, "Invalid option\n");
	  _exit(1);
	}
    }
  if(port_num <= 1024)
    {
      fprintf(stderr, "Error: Invalid port number or mandatory --port option not specified\n");
      _exit(1);
    }
  
  int server;
  if((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "Error: (server socket creation failed) %s\n", strerror(errno));
      _exit(1);
    }
  struct sockaddr_in server_addr;
  memset((char*) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_num);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  if((bind(server, (struct sockaddr*) &server_addr, sizeof(server_addr))) < 0)
    {
      fprintf(stderr, "Error: (bind failed) %s\n", strerror(errno));
      _exit(1);
    }
  if(listen(server, 5))
    {
      fprintf(stderr, "Error: (listen failed) %s\n", strerror(errno));
      _exit(1);
    }  
  struct sockaddr client_addr;
  int client_len = sizeof(client_addr);
  if((client = accept(server, &client_addr, (socklen_t * __restrict__) &client_len)) < 0)
    {
      fprintf(stderr, "Error: (accept failed) %s\n", strerror(errno));
      _exit(1);
    }
  int pipeA[2];
  int pipeB[2];
  int pid;
  if(pipe(pipeA) < 0)
    {
      fprintf(stderr, "Error creating pipe\n");
      close(client);
      _exit(1);
    }
  if(pipe(pipeB) < 0)
    {
      fprintf(stderr, "Error creating pipe\n");
      close(client);
      _exit(1);
    }
  pid = fork();
  if(pid < 0)
    {
      fprintf(stderr, "Error with fork\n");
      close(client);
      _exit(1);
    }
  else if(pid > 0)   //PARENT
    {
      signal(SIGPIPE, handler);
      close(pipeA[0]);   //https://github.com/raoulmillais/linux-system-programming/blob/master/src/poll-example.c
      close(pipeB[1]);
      struct pollfd rfds[2];
      rfds[0].fd = client;
      rfds[0].events = POLLIN | POLLHUP | POLLERR;
      rfds[1].fd = pipeB[0];
      rfds[1].events = POLLIN | POLLHUP | POLLERR;
      int status;
      int write_closed = 0;
      unsigned char shell_return[256];
      int flag = 0;
      int c_cflag = 0;
      unsigned char buffer[10];
      int num_read;
      unsigned char inf_out[11];
      unsigned char def_in[257];
      int end_status = 0;
      int exit;
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
	      if((num_read = read(client, buffer, 10)) < 0)
		{
		  fprintf(stderr, "Error: (read error) %s\n", strerror(errno));
		  close(client);
		  _exit(1);
		}

	      if(compress_flag)
		{
		  z_stream z_buffer;
		  z_buffer.zalloc = Z_NULL;
		  z_buffer.zfree = Z_NULL;
		  z_buffer.opaque = Z_NULL;
		  z_buffer.avail_in = num_read;
		  z_buffer.next_in = buffer;
		  if(inflateInit(&z_buffer))
		    {
		      fprintf(stderr, "Error: compress option initialization failed\r\n");
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		  while(z_buffer.avail_in > 0)
		    {
		      z_buffer.avail_out = 256;
		      z_buffer.next_out = inf_out;
		      if(inflate(&z_buffer, Z_SYNC_FLUSH))
			{
			  fprintf(stderr, "Error: inflate failed\r\n");
			  inflateEnd(&z_buffer);
			  _exit(1);
			}
		      int b_current = 0;
		      int inf_num = z_buffer.total_out;
		      for(; b_current < inf_num; b_current++)
			{
			  if(inf_out[b_current] == 0x03)
			    {
			      kill(pid, SIGINT);
			      c_cflag = 1;
			      break;
			    }
			  else if(inf_out[b_current] == 0x04)
			    {
			      flag = 1;
			      break;
			    }
			}
		      if(!write_closed && b_current && ((write(pipeA[1], inf_out, b_current)) < 0))
			{
			  fprintf(stderr, "Error: (write failed) %s\n", strerror(errno));
			  close(client);
			  _exit(1);
			}
		      if(flag)
			{
			  close(pipeA[1]);
			  write_closed = 1;   //Just in case
			} 
		    }
		  inflateEnd(&z_buffer);
		}
	      else
		{
		  int b_current = 0;
		  for(; b_current < num_read; b_current++)
		    {
		      if(buffer[b_current] == 0x03)
			{
			  kill(pid, SIGINT);
			  c_cflag = 1;
			  break;
			}
		      else if(buffer[b_current] == 0x04)
			{
			  flag = 1;
			  break;
			}
		    }
		  if(!write_closed && b_current && ((write(pipeA[1], buffer, b_current)) < 0))
		    {
		      fprintf(stderr, "Error: (write failed) %s\n", strerror(errno));
		      close(client);
		      _exit(1);
		    }
		  if(flag)
		    {
		      close(pipeA[1]);
		      write_closed = 1;   //Just in case
		    }
		}
	    }
	  
	  //SHELL READ SEGMENT
	  if(rfds[1].revents & POLLIN)
	    {
	      if((num_read = read(pipeB[0], shell_return, 256)) < 0)
		{
		  fprintf(stderr, "Error: (read error) %s\n", strerror(errno));
		  close(client);
		  _exit(1);
		}


	      if(compress_flag)
		{
		    //code referenced and used from "https://www.zlib.net/zpipe.c"
		  z_stream z_buffer;
		  z_buffer.zalloc = Z_NULL;
		  z_buffer.zfree = Z_NULL;
		  z_buffer.opaque = Z_NULL;
		  if(deflateInit(&z_buffer, 9))
		    {
		      fprintf(stderr, "Error: compress option initialization failed\r\n");
		      tcsetattr(0, TCSANOW, &stdin_init);
		      _exit(1);
		    }
		  z_buffer.avail_in = num_read;
		  z_buffer.next_in = shell_return;
		  z_buffer.avail_out = 256;
		  z_buffer.next_out = def_in;
		  if(deflate(&z_buffer, Z_SYNC_FLUSH))
		    {
		      fprintf(stderr, "Error: deflate failed\r\n");
		      deflateEnd(&z_buffer);
		      _exit(1);
		    }
		  num_read = z_buffer.total_out;
		  def_in[num_read] = '\0';
		  deflateEnd(&z_buffer);
		}
	      unsigned char *input = compress_flag ? def_in : shell_return;
	      if(num_read && ((write(client, input, num_read)) < 0))
		{
		  fprintf(stderr, "Error: (write failed) %s\n", strerror(errno));
		  close(client);
		  _exit(1);
		}
	    }
	  
	  //ERROR SEGMENT
	  if((rfds[1].revents & (POLLHUP | POLLERR)) && write_closed)
	    {
	      close(pipeB[0]);
	      break;
	    }    

	  if((exit = waitpid(pid, &end_status, WNOHANG)) < 0)
	    {
	      fprintf(stderr, "Error: waitpid failed\n");
	      close(client);
	      _exit(1);
	    }
	  if((exit == pid) && WIFEXITED(end_status))
	    {
	      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(end_status), WEXITSTATUS(end_status));
	      close(client);
	      _exit(0);
	    }
	}
      if(status < 0)
	{
	  fprintf(stderr, "Error: (poll error) %s\n", strerror(errno));
	  close(client);
	  _exit(1);
	}
      //reaps "zombie" child
      if((waitpid(pid, &end_status, 0)) < 0)
	{
	  fprintf(stderr, "Error: waitpid failed\n");
	  close(client);
	  _exit(1);
	}
      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(end_status), WEXITSTATUS(end_status));
    }
  else   //CHILD
    {
      close(pipeA[1]);
      close(pipeB[0]);
      close(server);
      close(client);
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
  close(client);
  _exit(0);
}
