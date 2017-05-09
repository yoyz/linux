#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>


struct timeval tv0;
struct timeval tv1;
struct timezone tz;
int64_t t;

int main()
{
        t=0;
        gettimeofday(&tv0,&tz);          
        sleep(1);
        gettimeofday(&tv1,&tz);          
        t+=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
        printf("sleeping 1s give : %ld\n",t);

}
