// need glibc install and launch this : /opt/glibc-2.17/bin/catchsegv ./stacktrace2
// gcc -g -O0                              stacktrace2.c                                  -o stacktrace2
#include<stdio.h>
#include<signal.h>
#include<unistd.h>



int main()
{
 memset((char *)0x0, 1, 2);
}
