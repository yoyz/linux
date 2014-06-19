#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h> 
#include <stdlib.h>



int main()
{
  int fd;  
  uint64_t * output_buffer=0;
  ssize_t    output_written=0;
  output_buffer=(uint64_t*)malloc((sizeof(uint64_t)/8)*1024*1024);
  
  if(output_buffer==NULL)
    { printf("can not allocate memory : %d\n",output_buffer); exit(1); }

  fd=open("filename",O_DIRECT|O_CREAT,S_IRWXU);
  if (!(fd>0))
    { printf("can not open filename open return : %d\n",fd); exit(1); }
  
  output_written=write(fd,output_buffer,(sizeof(uint64_t)*1024*1024);
  if(output_written<=0)
    { printf("can not write to filename : %d\n",output_written); exit(1); }

  close(fd);
  return 0;
}
