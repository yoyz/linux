#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void main(int argc,char ** argv)
{
        int fd;
        char * filename=argv[1];
        umask(022);
        //umask(O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO); 	// won't work
        fd=open(filename,O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
        //fd=open(filename,O_RDWR|O_CREAT); 	// if modes empty can lead to openat without args
        close(fd);
}

