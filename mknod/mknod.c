#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char **argv)
{

	mode_t mode = 0660;
	dev_t  d;
	int    major=0;
	int    minor=0;

	if (argc==3)
	{	
		char * ptn=argv[1];
		char * dev=argv[2];
		if (strcmp(dev,"S_IFREG")==0)  { mknod(ptn,mode,0); }
		if (strcmp(dev,"S_IFIFO")==0)  { mkfifo(ptn,mode);  }
		if (strcmp(dev,"S_IFSOCK")==0) { mknod(ptn,S_IFSOCK|mode,0); } 
	}
	else if (argc==5)
	{
		char * ptn=argv[1];
		char * dev=argv[2];
		if (strcmp(dev,"S_IFCHR")==0)  { major=atoi(argv[3]); minor=atoi(argv[4]); d=makedev(major,minor); mknod(ptn,S_IFCHR|mode,d); }
		if (strcmp(dev,"S_IFBLK")==0)  { major=atoi(argv[3]); minor=atoi(argv[4]); d=makedev(major,minor); mknod(ptn,S_IFBLK|mode,d); }
	}
	else
	{ 
		printf("%s <nodename> <[S_IFREG|S_IFIFO|S_IFSOCK]>\n",argv[0]); 
		printf("%s <nodename> <[S_IFCHR|S_IFBLK]> <major> <minor>\n",argv[0]); 
		exit(1); 
	}
}
