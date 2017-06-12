#include <stdio.h>
#include <stdint.h>
#include <unistd.h> //fork
#include <stdlib.h> //exit
#include <signal.h> //signal

#define STR_OMP_NUM_THREADS "16"
int32_t * buffer;
float quantity;
float time;

/*

void daemonize( void )
{   
    int64_t i;
    pid_t pid, sid;
    chdir("/tmp");
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    umask(0);
    for (i = 1024; i>=0; i--) { close (i); }
    sid = setsid();

    pid = fork();
    if( pid < 0)
        exit(1);
    else if( pid > 0)
        exit(0);
   pid = fork();
   if( pid < 0)
        exit(1);
    else if( pid > 0)
        exit(0);

}
*/
int main(int argc, char **argv)
{
        int64_t i=0;
        int64_t sizeByte;
        int64_t sizeFourByte;
        int64_t sizeMiByte;

        sizeByte=atoll(argv[1]);
        sizeFourByte=sizeByte/4;
        sizeMiByte=sizeByte/1024/1024;
        printf("balloon %lldMiB\n",sizeMiByte);
        buffer=malloc(sizeof(int32_t)*sizeFourByte);
        setenv("OMP_NUM_THREADS",STR_OMP_NUM_THREADS,0);
        #pragma omp parallel for
        for (i=0;i<sizeFourByte;i++)
        {
                buffer[i]=i;
        }
 //       daemonize();
        daemon(0,0);
        while(1)
                sleep(1);
}
