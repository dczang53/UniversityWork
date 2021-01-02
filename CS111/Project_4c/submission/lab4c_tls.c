#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <mraa/gpio.h>
#include <mraa/aio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int log_fd;
char *log_file = NULL;
char shutdown_str[] = "SHUTDOWN";
char time_str[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char print_str[50] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
time_t raw;
struct tm *tmp;

int main(int argc, char *argv[])
{
  struct option long_options[] = 
    {
      {"period", required_argument, 0, 'p'},
      {"scale", required_argument, 0, 's'},
      {"log", required_argument, 0, 'l'},
      {"id", required_argument, 0, 'i'},
      {"host", required_argument, 0, 'h'},
      {0, 0, 0, 0}
    };
  int f_scale = 1;
  unsigned int period = 1;
  int new;
  int state;
  int id_value = -1;
  char *id_string = NULL;
  char *host = NULL;
  int port_num = 0;
  while((state = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
      switch(state)
	{
	case 'p':
          new = atoi(optarg);
          if(new <= 0)
            {
              fprintf(stderr, "Invalid period\n");
              _exit(1);
            }
          period = (unsigned int) new;
	  break;
	case 's':
          if(strlen(optarg) != 1)
            {
	      fprintf(stderr, "Invalid option\n");
	      _exit(1);
            }
	  if(*optarg == 'F')
            f_scale = 1;
	  if(*optarg == 'C')
            f_scale = 0;
	  break;
	case 'l':
	  log_file = optarg;
	  break;
	case 'i':
	  id_value = atoi(optarg);
	  id_string = optarg;
	  break;
	case 'h':
	  host = optarg;
	  break;
	default:
	  fprintf(stderr, "Invalid option\n");
	  _exit(1);
	}
    }
  if(optind < argc)
    port_num = atoi(argv[optind]);
  if((optind + 1) < argc)
    {
      while((state = getopt_long(argc, &argv[optind + 1], "", long_options, NULL)) != -1)
	{
	  switch(state)
	    {
	    case 'p':
	      new = atoi(optarg);
	      if(new <= 0)
		{
		  fprintf(stderr, "Invalid period\n");
		  _exit(1);
		}
	      period = (unsigned int) new;
	      break;
	    case 's':
	      if(strlen(optarg) != 1)
		{
		  fprintf(stderr, "Invalid option\n");
		  _exit(1);
		}
	      if(*optarg == 'F')
		f_scale = 1;
	      if(*optarg == 'C')
		f_scale = 0;
	      break;
	    case 'l':
	      log_file = optarg;
	      break;
	    case 'i':
	      id_value = atoi(optarg);
	      id_string = optarg;
	      break;
	    case 'h':
	      host = optarg;
	      break;
	    default:
	      fprintf(stderr, "Invalid option\n");
	      _exit(1);
	    }
	}
    }
  if(log_file && ((log_fd = creat(log_file, 744)) < 0))
    {
      fprintf(stderr, "Creat failed\n");
      _exit(1);
    }
  period *= 1000000;
  if((id_value < 0) || (id_value > 999999999))
    {
      fprintf(stderr, "No id parameter or invalid id (must be 9 digit number)\n");
      _exit(1);
    }
  if(!host)
    {
      fprintf(stderr, "Must enter host name parameter\n");
      _exit(1);
    }
  int server;
  if((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "Error: (socket creation failed) %s\n", strerror(errno));
      _exit(1);
    }
  struct sockaddr_in client;
  struct hostent *name;
  if((name = gethostbyname(host)) == NULL)
    {
      fprintf(stderr, "Error: gethostbyname failed\n");
      _exit(1);
    }
  memset((char *) &client, 0, sizeof(&client));
  client.sin_family = AF_INET;
  client.sin_port = htons(port_num);
  memcpy((char *) &client.sin_addr.s_addr, (char *) name->h_addr, name->h_length);
  if(connect(server, (struct sockaddr *) &client, sizeof(client)))
    {
      fprintf(stderr, "Error: (connection failed) %s\n", strerror(errno));
      _exit(1);
    }
  mraa_aio_context sensor;
  sensor = mraa_aio_init(1);
  int running = 1;
  int start = 1;
  char *options[] = 
  {
    "SCALE=F",
    "SCALE=C",
    "PERIOD=",
    "STOP",
    "START",
    "LOG",
    "OFF",
    "SCALE=F\n",
    "SCALE=C\n",
    "STOP\n",
    "START\n",
    "OFF\n"
  };
  uint16_t s_read; 
  struct pollfd rfds;
  rfds.fd = server;
  rfds.events = POLLIN | POLLHUP | POLLERR;
  int status;
  char read_buffer[256];
  int num_read;
  char curr_string[256];
  int curr_len;
  int curr_offset;

  //CODE FOR SSL -> parts referenced to code by TA Haitao Zhang,  https://stackoverflow.com/questions/7698488/turn-a-simple-socket-into-an-ssl-socket, and http://h41379.www4.hpe.com/doc/83final/ba554_90007/ch04s03.html
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
  SSL_CTX *ssl_context = SSL_CTX_new(SSLv23_client_method());
  if(!ssl_context)
    {
      fprintf(stderr, "No SSL context\n");
      _exit(1);
    }
  SSL *ssl_server = SSL_new(ssl_context);
  if(!ssl_server)
    {
      fprintf(stderr, "No SSL structure\n");
      _exit(1);
    }
  if(!SSL_set_fd(ssl_server, server))
    {
      fprintf(stderr, "Setting fd failed\n");
      _exit(1);
    }
  if(SSL_connect(ssl_server) != 1)
    {
      fprintf(stderr, "No connection\n");
      _exit(1);
    }

  if(sprintf(print_str, "ID=%s\n", id_string) < 0)
    {
      fprintf(stderr, "Sprintf failed\n");
      SSL_shutdown(ssl_server);
      SSL_free(ssl_server);
      _exit(1);
    }
  if(SSL_write(ssl_server, print_str, strlen(print_str)) < 0)
    {
      fprintf(stderr, "Write failed (1)\n");
      SSL_shutdown(ssl_server);
      SSL_free(ssl_server);
      _exit(1);
    }
  if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
    {
      fprintf(stderr, "Logging failed\n");
      SSL_shutdown(ssl_server);
      SSL_free(ssl_server);
      _exit(1);
    }
  while((status = poll(&rfds, 1, 0)) >= 0)
    {
      if(rfds.revents & POLLIN)
	{
	  if((num_read = SSL_read(ssl_server, read_buffer, 256)) < 0)
	    {
	      fprintf(stderr, "Error: Read error (%s)\n", strerror(errno));
	      SSL_shutdown(ssl_server);
	      SSL_free(ssl_server);
	      _exit(1);
	    }
	  if(sscanf(read_buffer, "%[^\n]\n$*s", curr_string) != 1)
	    {
	      fprintf(stderr, "Sscanf failed\n");
	      SSL_shutdown(ssl_server);
	      SSL_free(ssl_server);
	      _exit(1);
	    }
          curr_len = strlen(curr_string);
          curr_offset = curr_len + 1;
	  while(curr_len)
            {
              if(strcmp(curr_string, options[0]) == 0)
                {
                  f_scale = 1;
                  if(log_file && (write(log_fd, options[7], 8) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                }
              else if(strcmp(curr_string, options[1]) == 0)
                {
                  f_scale = 0;
                  if(log_file && (write(log_fd, options[8], 8) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                }
              else if(strncmp(curr_string, options[2], 7) == 0)
                {
	          new = atoi(&curr_string[7]);
                  if(new <= 0)
                    {
	              fprintf(stderr, "Invalid period\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                  period = (unsigned int) new;
	          period *= 1000000;
                  if(sprintf(print_str, "PERIOD=%s\n", &curr_string[7]) < 0)
                    {
                      fprintf(stderr, "Sprintf failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
                      _exit(1);
                    }
                  if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                }
              else if(strcmp(curr_string, options[3]) == 0)
                {
                  start = 0;
                  if(log_file && (write(log_fd, options[9], 5) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                }
              else if(strcmp(curr_string, options[4]) == 0)
                {
                  start = 1;
                  if(log_file && (write(log_fd, options[10], 6) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	          }
                }
              else if(strncmp(curr_string, options[5], 3) == 0)
                {
		  if(sprintf(print_str, "%s\n", curr_string) < 0)
                    {
                      fprintf(stderr, "Sprintf failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
                      _exit(1);
                    }
                  if(log_file && (write(log_fd, print_str, curr_len + 1) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                }
              else if(strcmp(curr_string, options[6]) == 0)
                {
                  running = 0;
                  if(log_file && (write(log_fd, options[11], 4) < 0))
                    {
	              fprintf(stderr, "Logging failed\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
	            }
                }
              else
	        {
	          fprintf(stderr, "Invalid STDIN option\n");
		  SSL_shutdown(ssl_server);
		  SSL_free(ssl_server);
	          _exit(1);
	        }
              if((curr_offset) < num_read)
	        {
		  if((sscanf(read_buffer + curr_offset, "%[^\n]\n$*s", curr_string) != 1))
		    {
	              fprintf(stderr, "Sscanf failed redo\n");
		      SSL_shutdown(ssl_server);
		      SSL_free(ssl_server);
	              _exit(1);
		    }
                  curr_len = strlen(curr_string);
		  curr_offset += curr_len + 1;
                }
              else
	        break;
            }
	}

      if(running)
        {
          if(start)
            {
              time(&raw);
              tmp = localtime(&raw);
              if(strftime(time_str, 20, "%H:%M:%S ", tmp) == 0)
                {
                  fprintf(stderr, "Strftime failed\n");
		  SSL_shutdown(ssl_server);
		  SSL_free(ssl_server);
                  _exit(1);
                }
              s_read = mraa_aio_read(sensor);
              float R = 102300000.0/s_read-1.0;
              float temp = 1.0/(log(R/100000)/4275+1/298.15)-273.15;
              if(f_scale)
                temp = ((9/5) * temp) + 32;
              if(sprintf(print_str, "%s%.1f\n", time_str, temp) < 0)
                {
	          fprintf(stderr, "Sprintf failed\n");
		  SSL_shutdown(ssl_server);
		  SSL_free(ssl_server);
	          _exit(1);
	        }
              if(SSL_write(ssl_server, print_str, strlen(print_str)) < 0)
                {
	          fprintf(stderr, "Write failed\n");
		  SSL_shutdown(ssl_server);
		  SSL_free(ssl_server);
	          _exit(1);
	        }
              if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
		  SSL_shutdown(ssl_server);
		  SSL_free(ssl_server);
	          _exit(1);
	        }
            }
        }
      else
        {
          time(&raw);
          tmp = localtime(&raw);
          if(strftime(time_str, 20, "%H:%M:%S ", tmp) == 0)
            {
              fprintf(stderr, "Strftime failed\n");
	      SSL_shutdown(ssl_server);
	      SSL_free(ssl_server);
              _exit(1);
            }
          if(sprintf(print_str, "%s%s\n", time_str, shutdown_str) < 0)
            {
	      fprintf(stderr, "Sprintf failed\n");
	      SSL_shutdown(ssl_server);
	      SSL_free(ssl_server);
	      _exit(1);
	    }
          if(SSL_write(ssl_server, print_str, strlen(print_str)) < 0)
            {
	      fprintf(stderr, "Write failed\n");
	      SSL_shutdown(ssl_server);
	      SSL_free(ssl_server);
	      _exit(1);
	    }
          if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
            {
	      fprintf(stderr, "Logging failed\n");
	      SSL_shutdown(ssl_server);
	      SSL_free(ssl_server);
	      _exit(1);
	    }
          break;
        }
      if((rfds.revents & (POLLHUP | POLLERR)))
	{
	  break;
	}
      usleep(period);
    }
  if(status < 0)
    {
      fprintf(stderr, "Poll failed\n");
      SSL_shutdown(ssl_server);
      SSL_free(ssl_server);
      _exit(1);
    }
  SSL_shutdown(ssl_server);
  SSL_free(ssl_server);
  mraa_aio_close(sensor);
  _exit(0);
}
