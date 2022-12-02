#include <sys/types.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KB (1<<10)
#define MB (1<<20)
#define GB (1<<30)

int main(void)
{
        char *anon, *zero;
        int pid=getpid();
        for (int i=0;i<256*1024;i++)
        {
          anon = (char*)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
          if (i<32768)
            memset(anon,1,4096);
          if (i%4096==0) printf("%d\t%p\n",i,anon);
        }
        printf("/usr/bin/top -b -p %d\n",pid);
        sleep(3600);
}
