#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
 char * vbuf;
 FILE * F;
 int i;
 vbuf=malloc(sizeof(char)*1024*1024);

 F=fopen("/tmp/bla","w+");
 //setvbuf(F,vbuf,_IOFBF,1024*1024);
 
 for (i=0;i<123456;i++)
 	fprintf(F,"blablabla");
 fclose(F);

}
