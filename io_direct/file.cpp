#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h> 
#include <stdlib.h>

#include <malloc.h> // for memalign

#define ALIGNED 512

int main(int argc, char **argv)
{
  int fd;  
  uint64_t * output_buffer=0;
  ssize_t    output_written=0;
  int        firstwrite=0;

  if (!(argc>1))
    {
      printf("Need filename to output\n");
      printf("./file <filename>\n");
      exit(1);
    }
  
  //output_buffer=(uint64_t*)malloc((sizeof(uint64_t)/8)*1024*1024);
  output_buffer=(uint64_t*)memalign(ALIGNED,(sizeof(uint64_t)/8)*1024*1024);
  
  if(!output_buffer)
    { printf("can not allocate aligned memory : %d\n",output_buffer); exit(1); }

  fd=open(argv[1],O_DIRECT|O_RDWR|O_CREAT,S_IRWXU);
  if (!(fd>0))
    { printf("can not open filename open return : %d\n",fd); exit(1); }

  
  while(1)
    {
      lseek(fd,0,SEEK_SET);
      output_written=write(fd,output_buffer,(sizeof(uint64_t)/8)*1024*1024);
      if(output_written<=0)
	{ printf("can not write to filename : %d\n",output_written); exit(1); }
      if (firstwrite==0)
	{
	  printf("Writing to %s\n",argv[1]);
	  firstwrite=1;
	}
    }

  close(fd);
  return 0;
}
