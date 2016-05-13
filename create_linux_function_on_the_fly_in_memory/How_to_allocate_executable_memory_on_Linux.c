#include <stdio.h>
#include <sys/mman.h>

typedef unsigned (*asmFunc)(void);

int main(int argc, char *argv[])
{
   // probably needs to be page aligned...

   unsigned int codeBytes = 4096;
   void * virtualCodeAddress = 0;

   virtualCodeAddress = mmap(
       NULL,
       codeBytes,
       PROT_READ | PROT_WRITE | PROT_EXEC,
       MAP_ANONYMOUS | MAP_PRIVATE,
       0,
       0);

   printf("virtualCodeAddress = %p\n", virtualCodeAddress);

   // write some code in
   unsigned char * tempCode = (unsigned char *) (virtualCodeAddress);
   tempCode[0] = 0xb8;
   tempCode[1] = 0x00;
   tempCode[2] = 0x11;
   tempCode[3] = 0xdd;
   tempCode[4] = 0xee;
   // ret code! Very important!
   tempCode[5] = 0xc3;

   asmFunc myFunc = (asmFunc) (virtualCodeAddress);

   unsigned out = myFunc();

   printf("out is %x\n", out);

   return 0;
}
// http://burnttoys.blogspot.fr/2011/04/how-to-allocate-executable-memory-on.html
