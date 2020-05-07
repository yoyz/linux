#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char **argv)
{
  char * buff=NULL;
  int fd;
  off_t seek=4096;
  buff=malloc(sizeof(char)*4096);  
  memset(buff,1,4096);
  unlink("file");
  fd=open("file",O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
  lseek(fd,seek,SEEK_SET);
  write(fd,buff,4096);
  seek=seek*1024;
  lseek(fd,seek,SEEK_SET);
  write(fd,buff,4096);
  seek=seek*1024;
  lseek(fd,seek,SEEK_SET);
  write(fd,buff,4096);
  close(fd);
}
