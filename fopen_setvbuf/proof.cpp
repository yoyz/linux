#include <stdio.h>

int main(int argc, char ** argv)
{
	FILE * FP;
	char ptr[8]={'A'};
	int i=0;

	FP=fopen(argv[1],"w+");
	for(i=0;i<100000;i++)
		fwrite(ptr,8,1,FP);
	fclose(FP);
}
