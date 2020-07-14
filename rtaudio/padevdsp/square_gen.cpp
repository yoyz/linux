#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <cstdio>

int main()
{
  
  int i=0;
  int16_t buffer[4096];
  int counter=0;
  int count=0;
  while(1)
     {
       if (counter<100)
   	buffer[i]=-16000;
       if (counter>=100)
   	buffer[i]=16000;
       
       if (counter==200)
   	counter=0;
       if (i==4096)
	 {
	   int j;
	   int nbpos=0;
	   int nbneg=0;
	   for (j=0;j<4096;j++)
	     {
	       if (buffer[j]<0)
		 nbpos++;
	       else
		 nbneg++;
	     }
	       
		   
	   count=write(1,buffer,4096*sizeof(int16_t));
	   if (0)
	     fprintf(stderr,"squaregen : %d %d %d\n",count,nbpos,nbneg);
	   i=0;
	 }

       counter++;
       i++;
     }
}
