#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
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
#include <netdb.h>
#include "zlib.h"

//https://www.gnu.org/software/libc/manual/html_node/Host-Names.html

struct termios stdin_init;

int main(int argc, char *argv[])
{
  struct option long_options[] =
    {
      {"port", required_argument, 0, 'p'},
      {"log", required_argument, 0, 'l'},
      {"compress", no_argument, 0, 'c'},
      {0, 0, 0, 0}
    };
  int log_flag = 0;
  char *log_file;
  int port_num = 0;
  int compress_flag = 0;
  int state;
  while ((state = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
      switch (state)
	{
	case 'p':
	  port_num = atoi(optarg);
	  break;
	case 'l':
	  log_flag = 1;
	  log_file = optarg;
	  break;
	case 'c':
	  compress_flag = 1;
	  break;
	default:
	  fprintf(stderr, "Invalid option\n");
	  _exit(1);
	}
    }
  //deal with later
  if (port_num <= 1024)
    {
      fprintf(stderr, "Error: Invalid port number or mandatory --port option not specified\n");
      _exit(1);
    }
  
  if (tcgetattr(0, &stdin_init))
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
  if (tcsetattr(0, TCSANOW, &stdin_noncanon))
    {
      fprintf(stderr, "Error: tcsetattr failed (%s)\n", strerror(errno));
      _exit(1);
    }
  //tcsetattr(0, TCSANOW, &stdin_init);

  int client;
  if((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "Error: (socket creation failed) %s\r\n", strerror(errno));
      tcsetattr(0, TCSANOW, &stdin_init);
      _exit(1);
    }
  struct sockaddr_in server;
  struct hostent *name;
  if((name = gethostbyname("localhost")) == NULL)
    {
      fprintf(stderr, "Error: gethostbyname failed\r\n");
      tcsetattr(0, TCSANOW, &stdin_init);
      _exit(1);
    }
  memset((char *) &server, 0, sizeof(&server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port_num);
  memcpy((char *) &server.sin_addr.s_addr, (char *) name->h_addr, name->h_length);
  if(connect(client, (struct sockaddr *) &server, sizeof(server)))
    {
      fprintf(stderr, "Error: (connection failed) %s\r\n", strerror(errno));
      tcsetattr(0, TCSANOW, &stdin_init);
      _exit(1);
    }

  int log_fd;
  if(log_flag && ((log_fd = creat(log_file, 0744)) == -1))
    {
      fprintf(stderr, "Error: log file creation failed (%s)\r\n", strerror(errno));
      tcsetattr(0, TCSANOW, &stdin_init);
      _exit(1);
    }

  struct pollfd rfds[2];
  rfds[0].fd = 0;
  rfds[0].events = POLLIN;
  rfds[1].fd = client;
  rfds[1].events = POLLIN;
  int status;
  int write_closed = 0;
  unsigned char shell_input[11];
  shell_input[10] = '\0';
  unsigned char shell_return[257];
  shell_return[256] = '\0';
  unsigned char shell_print[512];
  int flag = 0;
  unsigned char buffer[10];
  unsigned char w_buffer[20];
  int num_read;

  char log_sent[] = "SENT ";
  char log_rec[] = "RECEIVED ";
  char log_bytes[] = " bytes: ";
  unsigned char log_sent_write[30];
  unsigned char log_rec_write[280];

  unsigned char shell_input2[11];
  unsigned char shell_return2[257];

  while((status = poll(rfds, 2, 0)) >= 0)
    {
      //READ SEGMENT
      if(rfds[0].revents & POLLIN)
	{
	  if((num_read = read(0, buffer, 10)) < 0)
	    {
	      fprintf(stderr, "Error: read error (%s)\r\n", strerror(errno));
	      tcsetattr(0, TCSANOW, &stdin_init);
	      if(log_flag)
		close(log_fd);
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
		  flag = 1;
		  w_buffer[w_current] = '^';
		  w_current++;
		  w_buffer[w_current] = 'C';
		  w_current++;
		  shell_input[shell] = buffer[b_current];
		  shell++;
		  break;
		}
	      else if(buffer[b_current] == 0x04)
		{
		  flag = 1;
		  w_buffer[w_current] = '^';
		  w_current++;
		  w_buffer[w_current] = 'D';
		  w_current++;
		  shell_input[shell] = buffer[b_current];
		  shell++;
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
	  shell_input[shell] = '\0';
	  if(!write_closed && shell)
	    {
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
		  z_buffer.avail_in = shell;
		  z_buffer.next_in = shell_input;
		  z_buffer.avail_out = 10;
		  z_buffer.next_out = shell_input2;
		  if(deflate(&z_buffer, Z_SYNC_FLUSH))
		    {
		      fprintf(stderr, "Error: deflate failed\r\n");
		      tcsetattr(0, TCSANOW, &stdin_init);
		      if(log_flag)
			close(log_fd);
		      deflateEnd(&z_buffer);
		      _exit(1);
		    }
		  shell = z_buffer.total_out;
		  shell_input2[shell] = '\0';
		  deflateEnd(&z_buffer);
		}
	      unsigned char *input = compress_flag ? shell_input2 : shell_input;
	      if((write(client, input, shell)) < 0)
		{
		  fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
		  tcsetattr(0, TCSANOW, &stdin_init);
		  if(log_flag)
		    close(log_fd);
		  _exit(1);
		}
	      if(log_flag)
		{
		  sprintf((char * __restrict__) log_sent_write, "%s%d%s%s\n", log_sent, shell, log_bytes, input);
		  if(write(log_fd, log_sent_write, strlen((const char *) log_sent_write)) < 0)
		    {
		      fprintf(stderr, "Error: log write failed (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      if(log_flag)
			close(log_fd);
		      _exit(1);
		    }
		}
	    }
	  if(w_current && ((write(1, w_buffer, w_current)) < 0))
	    {
	      fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
	      tcsetattr(0, TCSANOW, &stdin_init);
	      if(log_flag)
		close(log_fd);
	      _exit(1);
	    }
	  if(flag)
	    write_closed = 1;   //Just in case
	}
      
      //SHELL READ SEGMENT
      //https://sourceware.org/bugzilla/show_bug.cgi?id=13660
      if(rfds[1].revents & POLLIN)
	{
	  if((num_read = read(client, shell_return, 256)) < 0)
	    {
	      fprintf(stderr, "Error: read error (%s)\r\n", strerror(errno));
	      tcsetattr(0, TCSANOW, &stdin_init);
	      if(log_flag)
		close(log_fd);
	      _exit(1);
	    }
	  else if(!num_read)
	    break;
	  shell_return[num_read] = '\0';
	  if(log_flag)
	    {
	      sprintf((char * __restrict__) log_rec_write, "%s%d%s%s\n", log_rec, num_read, log_bytes, shell_return);
	      if(write(log_fd, log_rec_write, strlen((const char *) log_rec_write)) < 0)
		{
		  fprintf(stderr, "Error: log write failed (%s)\r\n", strerror(errno));
		  tcsetattr(0, TCSANOW, &stdin_init);
		  if(log_flag)
		    close(log_fd);
		  _exit(1);
		}
	    }
	  if(compress_flag)
	    {
	      z_stream z_buffer;
	      z_buffer.zalloc = Z_NULL;
	      z_buffer.zfree = Z_NULL;
	      z_buffer.opaque = Z_NULL;
	      z_buffer.avail_in = num_read;
	      z_buffer.next_in = shell_return;
	      if(inflateInit(&z_buffer))
		{
		  fprintf(stderr, "Error: compress option initialization failed\r\n");
		  tcsetattr(0, TCSANOW, &stdin_init);
		  _exit(1);
		}
	      while(z_buffer.avail_in > 0)
		{
		  z_buffer.avail_out = 256;
		  z_buffer.next_out = shell_return2;
		  if(inflate(&z_buffer, Z_SYNC_FLUSH))
		    {
		      fprintf(stderr, "Error: inflate failed\r\n");
		      tcsetattr(0, TCSANOW, &stdin_init);
		      if(log_flag)
			close(log_fd);
		      inflateEnd(&z_buffer);
		      _exit(1);
		    }
		  int w_current = 0;
		  int b_current = 0;
		  int infl_num = z_buffer.total_out;
		  for(; b_current < infl_num; b_current++)
		    {
		      if(shell_return2[b_current] == '\n')
			{
			  shell_print[w_current] = '\r';
			  w_current++;
			  shell_print[w_current] = '\n';
			  w_current++;
			}
		      else
			{
			  shell_print[w_current] = shell_return2[b_current];
			  w_current++;
			}
		    }
		  if(w_current && ((write(1, shell_print, w_current)) < 0))
		    {
		      fprintf(stderr, "Error: write failed (%s)\r\n", strerror(errno));
		      tcsetattr(0, TCSANOW, &stdin_init);
		      if(log_flag)
			close(log_fd);
		      inflateEnd(&z_buffer);
		      _exit(1);
		    }
		}
	      inflateEnd(&z_buffer);
	    }
	  else
	    {
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
		  if(log_flag)
		    close(log_fd);
		  _exit(1);
		}
	    }
	}
    }
  if(status < 0)
    {
      fprintf(stderr, "Error: poll error (%s)\r\n", strerror(errno));
      tcsetattr(0, TCSANOW, &stdin_init);
      if(log_flag)
	close(log_fd);
      _exit(1);
    }

  tcsetattr(0, TCSANOW, &stdin_init);
  if(log_flag)
    close(log_fd);
  _exit(0);
}
