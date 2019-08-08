// gcc -D_GNU_SOURCE  bigio.c -o bigio
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

//#define _GNU_SOURCE

#define BUFFER_SIZE 4*1024*1024

int main()
{
int f;
int size=BUFFER_SIZE;
int pagesize=getpagesize();
void * rb;
posix_memalign(&rb, pagesize, size);
char c[size];
f=open("bla",O_TRUNC|O_DIRECT|O_CREAT|O_WRONLY,S_IRWXU);
while(1) write(f,rb,size);
}
