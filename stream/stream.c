#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>


#define SIZEOFTOTALBUFFER 1000*1000*100
#define SIZEOFBUFFER SIZEOFTOTALBUFFER / 3
#define SIZE SIZEOFBUFFER / 4
#define TIMES                   100
int32_t bufferA[SIZE];
int32_t bufferB[SIZE];
int32_t bufferC[SIZE];
struct timeval tv0;
struct timeval tv1;
struct timezone tz;
int64_t t;
float quantity;
float time;

int main()
{
        int32_t i=0;
        int32_t times=0;
        t=0;
        printf("stream a[i]=b[i]+c[i] Buffer=%ld\n",SIZEOFTOTALBUFFER);
        #pragma omp parallel for
        for (i=0;i<SIZE;i++) 
	  {
	    bufferA[i]=i+1;
	    bufferB[i]=i+2;
	    bufferC[i]=i+3;
	  }

        gettimeofday(&tv0,&tz);
#pragma omp parallel for
        for (times=0;times<TIMES;times++)
	  {
	    for (i=0;i<SIZE;i++)
	      bufferA[i]=bufferB[i]+bufferC[i];
	  }
        gettimeofday(&tv1,&tz);
        t+=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
        time=t;
        time=time/1000000.0;
        quantity=(float)SIZEOFTOTALBUFFER*(float)TIMES;
	i=SIZE-1;
        printf("BW=%fMB/s t=%fs i=%d A=%ld B=%ld C=%ld\n",(quantity/time)/1000000,time,i,bufferA[i],bufferB[i],bufferC[i]);
}
