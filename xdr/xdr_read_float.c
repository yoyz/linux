#include <stdio.h>
#include <rpc/rpc.h> 

int main(int argc, char **argv)
{
  FILE * FD;
  //float a[4];
  //float b[4];
  float a;
  float b;
  int i;
  int j;
  int fd;
  XDR xdrs;

  FD=fopen("image_205_30.H@","r");
  //fd=fileno(FD);
  xdrstdio_create(&xdrs, FD, XDR_DECODE);
  //fread(
  //        size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
  for (j=0;j<501;j++)
  {
  	for (i=0;i<702;i++)
  	{
   //fread(&a,sizeof(float),1,FD);
   	if (!xdr_float(&xdrs,&a)) {
    	fprintf(stderr, "failed!\n");
        	         exit(1); }
  	 printf("%f \n",a);
   	}
        printf("\n");
  }
  fclose(FD);
}
