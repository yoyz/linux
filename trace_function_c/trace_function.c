// gcc  -D_GNU_SOURCE  -finstrument-functions -rdynamic -g  trace_function.c -o trace_function -ldl

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define __NIF__    __attribute__((__no_instrument_function__))
int __nbcall=0;
void __NIF__ __cyg_profile_func_enter (void *this_fn, void *call_site) 
{
  Dl_info info;
  int i;
  int res = dladdr(this_fn, &info);
  for (i=0;i<__nbcall;i++) printf(">");
  printf( " entering %p %s()\n", this_fn,info.dli_sname );
  __nbcall++;
  //(call_site);
}

void __NIF__  __cyg_profile_func_exit (void *this_fn, void *call_site) 
{
  Dl_info info;
  int i;
  int res = dladdr(this_fn, &info);
  for (i=0;i<__nbcall;i++) printf("<");
  printf( " leaving %p %s()\n", this_fn,info.dli_sname );
  __nbcall--;
  //(call_site);
}

int baz() { printf("hello\n"); }
int bar() { baz(); }
int foo() { bar(); }

void main(int argc, char **argv) { foo(); }
