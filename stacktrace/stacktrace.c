// https://www.gnu.org/software/libsigsegv/
// gcc -g -O0 -I /opt/libsigsegv/include/  stacktrace.c /opt/libsigsegv/lib/libsigsegv.a -o stacktrace
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sigsegv.h>

int handler (void *fault_address, int serious)
{
 //printf("catched\n");
 void* callstack[128];                                          
 int i, frames = backtrace(callstack, 128);                     
 char** strs = backtrace_symbols(callstack, frames);            
 for (i = 0; i < frames; ++i)
 {                                 
    printf("%s\n", strs[i]);                                   
 }                                                              
 printf("\n");                                                  
 exit(1);
}



int main()
{
 sigsegv_install_handler (&handler);
 memset((char *)0x0, 1, 2);
}
