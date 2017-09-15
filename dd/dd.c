#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main(int argc,char **argv)
{
	FILE *  FD;
	int64_t i;
	char *  filename;
	int64_t xfer;
	int64_t count;
	char *  buffer;
	if (argc < 4) { exit(1); }
	filename=argv[1];
	xfer=atoi(argv[2]);
	count=atoi(argv[3]);
	buffer=(char*)malloc(sizeof(char)*xfer);
	FD=fopen(filename,"w");
	for (i=0;i<count;i++)
		fwrite(buffer,xfer,1,FD);
	fclose(FD);	
}
