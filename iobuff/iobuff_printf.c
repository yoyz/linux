#include <stdio.h>

main ()
{
   char      *buf;
   FILE      *fp;
   long int  i=0;
   int       align=1<<20;
   fp = fopen ("data.fil", "w");
   buf = malloc (align);
   setvbuf (fp, buf, _IOFBF, align+8);
   while(i<1000000)
   {   
        fprintf(fp,"toto%i",i);
        i++;
   }
   fclose(fp);
}
