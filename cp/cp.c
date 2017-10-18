#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define XFER 1048576
int main(int argc,char **argv)
{
	FILE *  FD_src;
	FILE *  FD_dst;
	int64_t i;
	char *  filename_src;
	char *  filename_dst;
	int64_t xfer=XFER;
	int64_t count;
	char *  buffer;
	char *  vbuff_src;
	char *  vbuff_dst;
	
	int size;
	if (argc < 3) { exit(1); }
	filename_src=argv[1];
	filename_dst=argv[2];
	buffer=(char*)malloc(sizeof(char)*xfer);
	FD_src=fopen(filename_src,"r");
	FD_dst=fopen(filename_dst,"w");

	vbuff_src = malloc (XFER*64);
	setvbuf (FD_src, vbuff_src, _IOFBF, XFER*64);
	vbuff_dst = malloc (XFER*64);
	setvbuf (FD_dst, vbuff_dst, _IOFBF, XFER*64);


	while (feof(FD_src)==0)
		{
			size=fread(buffer,1,xfer,FD_src);
			fwrite(buffer,1,size,FD_dst);
		}
	fclose(FD_src);	
	fclose(FD_dst);	
}
