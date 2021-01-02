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

int log_fd;
char *log_file = NULL;
char shutdown[] = "SHUTDOWN";
char time_str[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char print_str[50] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
time_t raw;
struct tm *tmp;
void interruption()
{
  time(&raw);
  tmp = localtime(&raw);
  if(strftime(time_str, 20, "%H:%M:%S ", tmp) == 0)
    {
      fprintf(stderr, "Strftime failed\n");
      _exit(1);
    }
  if(sprintf(print_str, "%s%s\n", time_str, shutdown) < 0)
    {
      fprintf(stderr, "Sprintf failed\n");
      _exit(1);
    }
  if(printf("%s", print_str) < 0)
    {
      fprintf(stderr, "Printf failed\n");
      _exit(1);
    }
  if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
    {
      fprintf(stderr, "Logging failed\n");
      _exit(1);
    }
  _exit(0);
}

int main(int argc, char *argv[])
{
  struct option long_options[] = 
    {
      {"period", required_argument, 0, 'p'},
      {"scale", required_argument, 0, 's'},
      {"log", required_argument, 0, 'l'},
      {0, 0, 0, 0}
    };
  int f_scale = 1;
  unsigned int period = 1;
  int new;
  int state;
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
	default:
	  fprintf(stderr, "Invalid option\n");
	  _exit(1);
	}
    }
  if(log_file && ((log_fd = creat(log_file, 744)) < 0))
    {
      fprintf(stderr, "Creat failed\n");
      _exit(1);
    }
  period *= 1000000;
  mraa_gpio_context button;
  button = mraa_gpio_init(62);
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &interruption, NULL);
  mraa_aio_context sensor;
  sensor = mraa_aio_init(1);
  int running = 1;
  int start = 1;
  char *options[] = 
  {
    "SCALE=F\n",
    "SCALE=C\n",
    "PERIOD=",
    "STOP\n",
    "START\n",
    "LOG",
    "OFF\n",
  };
  uint16_t s_read; 
  struct pollfd rfds;
  rfds.fd = 0;
  rfds.events = POLLIN | POLLHUP | POLLERR;
  int status;
  char *read_buffer = malloc(256 * sizeof(char));
  size_t b_size = 256;
  int num_read;
  while((status = poll(&rfds, 1, 0)) >= 0)
    {
      if(rfds.revents & POLLIN)
	{
	  if((num_read = getline(&read_buffer, &b_size, stdin)) < 0)
	    {
	      fprintf(stderr, "Error: Getline error (%s)\n", strerror(errno));
	      _exit(1);
	    }
          if(strcmp(read_buffer, options[0]) == 0)
            {
              f_scale = 1;
              if(log_file && (write(log_fd, options[0], 8) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else if(strcmp(read_buffer, options[1]) == 0)
            {
              f_scale = 0;
              if(log_file && (write(log_fd, options[1], 8) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else if(strncmp(read_buffer, options[2], 7) == 0)
            {
              read_buffer[num_read-1] = '\0';
	      new = atoi(&read_buffer[7]);
              if(new <= 0)
                {
	          fprintf(stderr, "Invalid period\n");
	          _exit(1);
	        }
              period = (unsigned int) new;
	      period *= 1000000;
              read_buffer[num_read-1] = '\n';
              if(log_file && (write(log_fd, read_buffer, strlen(read_buffer)) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else if(strcmp(read_buffer, options[3]) == 0)
            {
              start = 0;
              if(log_file && (write(log_fd, options[3], 5) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else if(strcmp(read_buffer, options[4]) == 0)
            {
              start = 1;
              if(log_file && (write(log_fd, options[4], 6) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else if(strncmp(read_buffer, options[5], 3) == 0)
            {
/*
              if(read_buffer[3] != ' ' && read_buffer[3] != '\n')
                {
	          fprintf(stderr, "Invalid STDIN option\n");
	          _exit(1);
	        }
		*/
              if(log_file && (write(log_fd, read_buffer, strlen(read_buffer)) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else if(strcmp(read_buffer, options[6]) == 0)
            {
              running = 0;
              if(log_file && (write(log_fd, options[6], 4) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
	          _exit(1);
	        }
            }
          else
	    {
	      fprintf(stderr, "Invalid STDIN option\n");
	      _exit(1);
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
	          _exit(1);
	        }
              if(printf("%s", print_str) < 0)
                {
	          fprintf(stderr, "Printf failed\n");
	          _exit(1);
	        }
              if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
                {
	          fprintf(stderr, "Logging failed\n");
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
              _exit(1);
            }
          if(sprintf(print_str, "%s%s\n", time_str, shutdown) < 0)
            {
	      fprintf(stderr, "Sprintf failed\n");
	      _exit(1);
	    }
          if(printf("%s", print_str) < 0)
            {
	      fprintf(stderr, "Printf failed\n");
	      _exit(1);
	    }
          if(log_file && (write(log_fd, print_str, strlen(print_str)) < 0))
            {
	      fprintf(stderr, "Logging failed\n");
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
      _exit(1);
    }

  mraa_gpio_close(button);
  mraa_aio_close(sensor);
  free(read_buffer);
  _exit(0);
}

