#include <fftw3.h>
#include <math.h>
#include <stdint.h>
#define N 24

int main()
{
  int64_t                i;
  fftw_complex * in, * out;
  fftw_plan              p;

  in  =  (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
  out =  (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

  p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

  // create a sample of cos in 24 sample
  for (i=0;i<N;i++)
    (*in[i])=cos((2.0*3.14159*(float)i)/(N/2));

  fftw_execute(p);

  printf("IN : ");
  for (i=0;i<N;i++)
    printf("%.4f ",*in[i]);
  printf("\n");
  printf("OUT: ");
  for (i=0;i<N;i++)
    printf("%.4f ",*out[i]);

  fftw_destroy_plan(p);
  fftw_free(in); 
  fftw_free(out);
}
