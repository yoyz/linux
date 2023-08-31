#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

char * big_buf1;
char * big_buf2;
char * file_buf;
ssize_t byteread;
int fd;
 
void memstat()
{
  fd=open("/proc/self/status",O_RDONLY); while((byteread=read(fd,file_buf,4096))>0) { write(STDOUT_FILENO,file_buf,byteread); } close(fd);
}

void main()
{
  big_buf1=(void*)malloc(sizeof(char)*1024*1024*1024);
  big_buf2=(void*)malloc(sizeof(char)*1024*1024*1024);
  file_buf=(void*)malloc(sizeof(char)*4096);

  memstat();
  memset(big_buf1, 1, sizeof(char)*1024*1024*1024);
  memstat();
  memset(big_buf2, 1, sizeof(char)*1024*1024*1024);
  memstat();
  free(big_buf1);
  memstat();
  free(big_buf2);
  memstat();
}
