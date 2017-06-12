#include <stdio.h>
#include <stdint.h>
#include <unistd.h> //fork
#include <stdlib.h> //exit

int32_t * buffer;
float quantity;
float time;

void daemonize( void )
{
    pid_t pid, sid;
    pid = fork();
    if( pid < 0)
        exit(1);
    else if( pid > 0)
        exit(0);
    umask(0);
    sid = setsid();
    if( sid < 0 )
    {
        perror( "daemonize::sid" );
        exit(1);
    }
    if( chdir("/") < 0 )
    {
        perror( "daemonize::chdir" );
        exit( 1);
    }
 
    close( STDIN_FILENO );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );
}

int main(int argc, char **argv)
{
        int64_t i=0;
	int64_t sizeByte;
	int64_t sizeFourByte;
	int64_t sizeMiByte;

	sizeByte=atoi(argv[1]);
	sizeFourByte=sizeByte/4;
	sizeMiByte=sizeByte/1024/1024;
        printf("balloon %lldMiB\n",sizeMiByte);
	buffer=malloc(sizeof(int32_t)*sizeFourByte);
        #pragma omp parallel for
        for (i=0;i<sizeFourByte;i++)
        {
                buffer[i]=i;
        }
	daemonize();
	while(1)
		sleep(1);
}
