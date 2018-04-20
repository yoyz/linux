#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>


#define RAMTOUSE            1000*1000*300
#define BUFFERINBYTE        RAMTOUSE        / 3
#define NBITEM              BUFFERINBYTE    / 4
#define TIMES               2000
//float   volatile bufferA[SIZE];
//float   volatile bufferB[SIZE];
//float   volatile bufferC[SIZE];
int32_t bufferA[NBITEM];
int32_t bufferB[NBITEM];
int32_t bufferC[NBITEM];

struct timeval tv0;
struct timeval tv1;
struct timezone tz;
float quantity;
float time;

int main()
{
        int64_t t=0;
        int64_t i=0;
        int64_t times=0;
        t=0;
        printf("stream a[i]=b[i]+c[i] Buffer=%ld\n",RAMTOUSE);
        #pragma omp parallel for
        for (i=0;i<NBITEM;i++) 
	  {
	    bufferA[i]=i;
	    bufferB[i]=i+1;
	    bufferC[i]=i+2;
	  }

        gettimeofday(&tv0,&tz);
        for (times=0;times<TIMES;times++)
	  {
        #pragma omp parallel for
	    for (i=0;i<NBITEM;i++)
	      bufferA[i]=bufferB[i]+bufferC[i];
	  }
        gettimeofday(&tv1,&tz);
        t=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
        time=t;
        time=time/1000000.0;
        quantity=(float)RAMTOUSE*(float)TIMES;
	    i=NBITEM-1;
        printf("BW=%fMB/s t=%fs i=%d A=%ld B=%ld C=%ld\n",(quantity/time)/1000000,time,i,bufferA[i],bufferB[i],bufferC[i]);
}
