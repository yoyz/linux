#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <fcntl.h>


int main()
{
  off_t start=0;
  off_t len=0;
  unlink("test");
  int fd=open("test",O_CREAT|O_RDWR,0666);
  posix_fallocate(fd,0,(off_t)1024*1024*1024*100);
  close(fd);
}
