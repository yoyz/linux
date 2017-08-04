#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ELEMTOMALLOC 1024*1024

int main()
{
        int64_t * buf;
        //int   lock_h;
        int retcode;
        buf=malloc(ELEMTOMALLOC*sizeof(int64_t));
        if (buf!=0)
                printf("malloc success\n");
        retcode=mlock(buf,ELEMTOMALLOC*sizeof(int64_t));
        if (retcode==0)
                printf("mlock success\n");
        else
                printf("mlock fail\n");
        sleep(60);
        free(buf);


}

