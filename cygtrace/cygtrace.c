#ifndef __CYGPROF__
#define __CYGPROF__
#include <dlfcn.h>

#define __NIF__    __attribute__((__no_instrument_function__))
int __nbcall=0;
void __NIF__ __cyg_profile_func_enter (void *this_fn, void *call_site)
{ 
  Dl_info info;
  int i;
  dladdr(this_fn, &info);
  for (i=0;i<__nbcall;i++) printf(">");
  printf( " entering %p %s()\n", this_fn,info.dli_sname );
  __nbcall++;
  //(call_site);
}

void __NIF__  __cyg_profile_func_exit (void *this_fn, void *call_site)
{ 
  Dl_info info;
  int i;
  dladdr(this_fn, &info);
  for (i=0;i<__nbcall;i++) printf("<");
  printf( " leaving %p %s()\n", this_fn,info.dli_sname );
  __nbcall--;
  //(call_site);
}
#endif
