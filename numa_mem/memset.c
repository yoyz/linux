#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
        char * buff=malloc(10*1024*1024);
        memset(buff,1,     10*1024*1024);
        sleep(100000);
}

