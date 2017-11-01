#include "FixedPoint32.h"
#include "math.h"

#define SIZE 32
int main()
{
  printf("FixedPoint\n");
  FixedPoint a1,b1,c1;
  FixedPoint a2,b2,c2;
  FixedPoint a3,b3,c3;
  FixedPoint a4,b4,c4;
  
  FixedPoint *tab_fx=(FixedPoint*)malloc(sizeof(FixedPoint)*SIZE);
  float      *tab_f=(float*)malloc(sizeof(float)*SIZE);
  a1=FixedPoint(float(1.24));
  b1=FixedPoint(float(2.33));
  c1=a1+b1;

  a2=FixedPoint(float(1.24));
  b2=FixedPoint(float(2.33));
  c2=a2/b2;

  a3=FixedPoint(float(1.24));
  b3=FixedPoint(float(2.33));
  c3=a3-b3;

  a4=1;
  b4=2;
  c4=a4+b4;
  
  for (int i=0;i<SIZE;i++)
    {
      tab_fx[i]=float(sin((3.14159)/float(i)));
      tab_f[i]=sin((3.14159)/float(i));
    }
  
  printf("f: %f %f %f %f\n",float(c1),float(c2),float(c3),float(c4));
  printf("d: %d %d %d %d\n",int(c1),int(c2),int(c3),int(c4));
  

  for (int i=2;i<SIZE;i++)
      printf("%f ",float(tab_fx[i]));
  printf("\n");
  
  for (int i=2;i<SIZE;i++)
      printf("%f ",float(tab_f[i]));
  printf("\n");
	      
  
}
