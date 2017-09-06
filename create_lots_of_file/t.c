#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUFFERSIZE 10000
struct timeval tv0;
struct timeval tv1;
struct timezone tz;


int main(int argc,char **argv)
{
  int i;
  int s;
  int fd;
  char str[128];
  char buffer[BUFFERSIZE];
  for (s=0;s<2;s++)
    {
      for (i=0;i<1000;i++)
        {
          sprintf(str,"%s/%d",argv[1],i);
          fd=open(str,O_WRONLY|O_CREAT);
          write(fd,buffer,BUFFERSIZE);
          close(fd);
        }
    }
}
