#define MAX 10000
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void swapme(uint16_t * tosort,int x,int y)
{
  uint16_t swapvalue;
  swapvalue=tosort[x];
  tosort[x]=tosort[y];
  tosort[y]=tosort[x];
}


int main()
{
  uint16_t tosort[MAX];
  //uint16_t swap;
  int i;
  int j;

  srand(12);
  for (i=0;i<MAX;i++)
    tosort[i]=(uint16_t)rand();

  for (j=0;j<MAX;j++)
    for (i=0;i<MAX;i++)
      if (tosort[i]>tosort[i+1])
	swapme(tosort,i,i+1);
	/*
	{
	  swap=tosort[i+1];
	  tosort[i+1]=tosort[i];
	  tosort[i]=swap;
	  }*/
  
  for (i=0;i<MAX;i++)
    printf("%d ",tosort[i]);
}
