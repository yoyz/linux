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

//#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "vectorclass.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

long long max1=100000;
long long max2=20000;

double stoptime(void) 
{
  struct timeval t;
  gettimeofday(&t,NULL);
  return (double) t.tv_sec + t.tv_usec/1000000.0;
}

double addmul_avx()
{
  long long ii;
  double  res;
  double  mul=1.00001;

  Vec4d  a(0.1, 0.2, 0.3, 0.4);
  Vec4d  b(0.5, 0.6, 0.7, 0.8);
  Vec4d  c(0.9, 1.0, 1.1, 1.2);
  Vec4d  d(1.3, 1.4, 1.5, 1.6);

  Vec4d  e(2.1, 2.2, 2.3, 2.4);
  Vec4d  f(2.5, 2.6, 2.7, 2.8);
  Vec4d  g(2.9, 3.0, 3.1, 3.2);
  Vec4d  h(3.3, 3.4, 3.5, 3.6);

  Vec4d  i(4.1, 4.2, 4.3, 4.4);
  Vec4d  j(4.5, 4.6, 4.7, 4.8);
  Vec4d  k(4.9, 5.0, 5.1, 5.2);
  Vec4d  l(5.3, 5.4, 5.5, 5.6);

  Vec4d  m(6.1, 6.2, 6.3, 6.4);
  Vec4d  n(6.5, 6.6, 6.7, 6.8);
  Vec4d  o(6.9, 7.0, 7.1, 7.2);
  Vec4d  p(7.3, 7.4, 7.5, 7.6);

  
  for(ii=0; ii< max1*max2 ; ii++)
    {
      a=a*c;
      b=b+d;
      c=c*e;
      d=d+f;
   
      e=e*g;
      f=f+h;
      g=g*i;
      h=h+j;

      i=i*k;
      j=j+l;
      k=k*m;
      l=l+n;

      m=m*o;
      n=n+p;
      o=o*a;
      p=p+b;
    }

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

  return res;

}



double addmul_sse()
{
  long long ii;
  double    res;
  double    mul=1.00001;

  Vec2d  a(0.1, 0.2);
  Vec2d  b(0.3, 0.4);
  Vec2d  c(0.5, 0.6);
  Vec2d  d(0.7, 0.8);

  Vec2d  e(2.1, 2.2);
  Vec2d  f(2.3, 2.4);
  Vec2d  g(2.5, 2.6);
  Vec2d  h(2.7, 2.8);

  Vec2d  i(3.1, 3.2);
  Vec2d  j(3.3, 3.4);
  Vec2d  k(3.5, 3.6);
  Vec2d  l(3.7, 3.8);

  Vec2d  m(4.1, 4.2);
  Vec2d  n(4.3, 4.4);
  Vec2d  o(4.5, 4.6);
  Vec2d  p(4.7, 4.8);

  
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
  int size, rank;
  char hostname[1024];

  if (argc > 1) { iter = atoi(argv[1]); }

  //  MPI_Init(&argc, &argv);
  //  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //  MPI_Comm_size(MPI_COMM_WORLD, &size);


  hostname[1023] = '\0';
  gethostname(hostname, 1023);
  t=stoptime();  
  /*
#ifdef __SSE__
  printf("calling addmul_sse\n");
  for (i=0;i<iter;i++)
    x+=addmul_sse();
#endif

  */


#ifdef __AVX__
  printf("calling addmul_avx\n");
  printf("AVX\n");
  for (i=0;i<iter;i++)
    x+=addmul_avx();
#endif


  t=stoptime()-t;

  // Here we launch max1*max2*iteration 
  // 16 assembly instruction on 16 register
  // storing 2 data on SSE
  // storing 4 data on AVX
  /*
#ifdef __SSE__
    printf("host: %s\tgflops:\t %.3f s, %.3f Gflops, rank: %.4d res=%f\n",         
         hostname,
         t,
         (double)max1*max2*iter*16*2/t/1e9,
         rank,
         x);
#endif
  */

#ifdef __AVX__
  printf("host: %s\tgflops:\t %.3f s, %.3f Gflops, rank: %.4d res=%f\n",
         hostname,
         t,
         (double)max1*max2*iter*16*4/t/1e9,
         rank,
         x);
#endif

  //  MPI_Finalize();

  return 0;
}

