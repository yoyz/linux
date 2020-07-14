#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <cstdio>

int main()
{
  int16_t buffer[4096];
  int16_t nBufferFrames=256;
  int16_t count;
  while(1)
    {      
      int j=0;
      int nbpos=0;
      int nbneg=0;

      count=read(0,buffer,nBufferFrames*sizeof(int16_t));
      
      for (j=0;j<nBufferFrames;j++)
	{
	  if (buffer[j]<0)
	    nbpos++;
	  else
	    nbneg++;
	}
  
		   
      //count=write(1,buffer,4096);
      //fprintf(stderr,"squarerecv : %d %d %d\n",count,nbpos,nbneg);

    }
}
