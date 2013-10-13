//#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

//#include "vectorclass.h"

using namespace std;

#define  EXIT_FAILURE 1
#define  EXIT_SUCCESS 0

long long max1=100;
long long max2=1000;

double stoptime(void) {
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}
/*
double addmul()
{
  long long ii;
  double  res;
  double  mul=1.00001;
  //  float  mul=1.00001;
  /*
  Vec4d  a(0.1, 0.2, 0.3, 0.4);
  Vec4d  b(0.5, 0.6, 0.7, 0.8);
  Vec4d  c(1.1, 1.2, 1.3, 1.4);
  Vec4d  d(1.5, 1.6, 1.7, 1.8);
  Vec4d  e(2.1, 2.2, 2.3, 2.4);
  Vec4d  f(3.1, 3.2, 3.3, 3.4);
  Vec4d  g(4.1, 4.2, 4.3, 4.4);
  Vec4d  h(5.1, 5.2, 5.3, 5.4);

  Vec4d  a(0.1, 0.2, 0.3, 0.4);
  Vec4d  b(0.5, 0.6, 0.7, 0.8);
  Vec4d  c(1.1, 1.2, 1.3, 1.4);
  Vec4d  d(1.5, 1.6, 1.7, 1.8);

  Vec4d  e(2.1, 2.2, 2.3, 2.4);
  Vec4d  f(3.1, 3.2, 3.3, 3.4);
  Vec4d  g(4.1, 4.2, 4.3, 4.4);
  Vec4d  h(5.1, 5.2, 5.3, 5.4);

  Vec4d  i(6.1, 6.2, 6.3, 6.4);
  Vec4d  j(7.1, 7.2, 7.3, 7.4);
  Vec4d  k(8.1, 8.2, 8.3, 8.4);
  Vec4d  l(9.1, 9.2, 9.3, 9.4);

  Vec4d  m(13.1, 13.2, 13.3, 13.4);
  Vec4d  n(12.1, 12.2, 12.3, 12.4);
  Vec4d  o(11.1, 11.2, 11.3, 11.4);
  Vec4d  p(10.1, 10.2, 10.3, 10.4);

  
  //Vec4d  b(1.1, 1.2, 1.3, 1.4);
  for(ii=0; ii< max1*max2 ; ii++)
    {
      a=a*mul;
      b=b+mul;
      c=c*mul;
      d=d+mul;
   
      e=e*mul;
      f=f+mul;
      g=f*mul;
      h=f+mul;

      i=i*mul;
      j=j+mul;
      k=k*mul;
      l=l+mul;

      m=m*mul;
      n=n+mul;
      o=o*mul;
      p=p+mul;

    }
  //  return a[0]+a[1]+a[2]+a[3];
  res=
    a[0]+a[1]+a[2]+a[3] + 
    b[0]+b[1]+b[2]+b[3] + 
    c[0]+c[1]+c[2]+c[3] +
    d[0]+d[1]+d[2]+d[3] +

    e[0]+e[1]+e[2]+e[3] + 
    f[0]+f[1]+f[2]+f[3] + 
    g[0]+g[1]+g[2]+g[3] +
    h[0]+h[1]+h[2]+h[3] +

    i[0]+i[1]+i[2]+i[3] + 
    j[0]+j[1]+j[2]+j[3] + 
    k[0]+k[1]+k[2]+k[3] +
    l[0]+l[1]+l[2]+l[3] +

    m[0]+m[1]+m[2]+m[3] + 
    n[0]+n[1]+n[2]+n[3] + 
    o[0]+o[1]+o[2]+o[3] +
    p[0]+p[1]+p[2]+p[3] 

  ;

    //    f[0]+f[1]+f[2]+f[3] +
    //    e[0]+e[1]+e[2]+e[3];
  return res;

}
*/

double addmul()
{
  float  mul1=1.1,mul2=1.2,mul3=1.3,mul4=1.4,mul5=1.5,mul6=1.6,mul7=1.7,mul8=1.8;
  float  mul=3.14;
  long long i,j;
  for(i=0; i< max1*max2 ; i++)
    //        {  mul1/=mul; mul2/=mul; mul3/=mul; mul4/=mul; mul5/=mul; mul6/=mul; mul7/=mul; mul8/=mul; }
        {  mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul; mul6*=mul; mul7*=mul; mul8*=mul; }
  return  mul1+mul2+mul3+mul4+mul5+mul6+mul7+mul8;
}


int main(int argc, char** argv)
{
  double t;
  double x;
  double i=0;
  double iter=1;
  int size=0;
  int rank=0;
  char hostname[1024];

  if (argc > 1) { iter = atoi(argv[1]); }
  else
    iter=1;
  /*
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  */

  hostname[1023] = '\0';
  gethostname(hostname, 1023);
  t=stoptime();  
  for (i=0;i<iter;i++)
    x+=addmul();
  t=stoptime()-t;
  printf("rank: %.4d\thost: %s\ttime\t %.3f s, %.3f Kflops, rank: %.4d res=%f\n",
         rank,
         hostname,
         t,
         (double)max1*max2*iter*8/t/1e3,
         1,
         x);

  //  MPI_Finalize();

  return 0;
}

