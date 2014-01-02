/*
 * gflops.c
 *
 * Copyright (c) 2011-1012 Peyrard Johann <peyrard.johann@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JOHANN PEYRARD ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#include "vectorclass.h"

using namespace std;

#define  EXIT_FAILURE 1
#define  EXIT_SUCCESS 0

long long max1=10000;
long long max2=10000;

double stoptime(void) {
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}

double addmul_sse()
{
  long long ii;
  double    res;
  double    mul=1.00001;

  Vec2d  a(0.1, 0.2);
  Vec2d  b(0.5, 0.6);
  Vec2d  c(0.9, 1.0);
  Vec2d  d(1.3, 1.4);

  Vec2d  e(2.1, 2.2);
  Vec2d  f(2.5, 2.6);
  Vec2d  g(2.9, 3.0);
  Vec2d  h(3.3, 3.4);

  Vec2d  i(4.1, 4.2);
  Vec2d  j(4.5, 4.6);
  Vec2d  k(4.9, 5.0);
  Vec2d  l(5.3, 5.4);

  Vec2d  m(6.1, 6.2);
  Vec2d  n(6.5, 6.6);
  Vec2d  o(6.9, 7.0);
  Vec2d  p(7.3, 7.4);

  
  for(ii=0; ii< max1*max2 ; ii++)
    {
      a=a*b;
      b=b+c;
      c=c*d;
      d=d+e;
   
      e=e*f;
      f=f+g;
      g=g*h;
      h=h+i;

      i=i*j;
      j=j+k;
      k=k*l;
      l=l+m;

      m=m*n;
      n=n+o;
      o=o*p;
      p=p+a;
    }

  res=
    a[0]+a[1]+
    b[0]+b[1]+
    c[0]+c[1]+
    d[0]+d[1]+

    e[0]+e[1]+
    f[0]+f[1]+
    g[0]+g[1]+
    h[0]+h[1]+

    i[0]+i[1]+
    j[0]+j[1]+
    k[0]+k[1]+
    l[0]+l[1]+

    m[0]+m[1]+
    n[0]+n[1]+
    o[0]+o[1]+
    p[0]+p[1]

  ;

  return res;

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
    x+=addmul_sse();
  t=stoptime()-t;
  printf("rank: %.4d\thost: %s\ttime\t %.3f s, %.9f GFlops, rank: %.4d res=%f\n",
         rank,
         hostname,
         t,
         (double)max1*max2*iter*16*2/t/1e9,
         1,
         x);

  //  MPI_Finalize();

  return 0;
}

