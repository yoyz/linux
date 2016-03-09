#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef int (*orig_open_f_type)(const char *pathname, int flags);
 
int open(const char *pathname, int flags, ...)
{
  printf("open(%s,%d) \n",pathname,flags);
  orig_open_f_type orig_open;
  orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  return orig_open(pathname,flags);
}

static void con() __attribute__((constructor));

void con()
{
  FILE * F;
  pid_t pid; 
  char  log[1024];

  pid=getpid();
  sprintf(log,"/tmp/const.%d",pid);
  F=fopen(log,"w");
  fprintf(F,"I'm a constructor %d\n",pid);
  fclose(F);
}


static void des() __attribute__((destructor));

void des()
{
  FILE * F;
  pid_t pid; 
  char  log[1024];

  pid=getpid();
  sprintf(log,"/tmp/const.%d",pid);
  F=fopen(log,"a+");
  fprintf(F,"I'm the destructor %d\n",pid);
  fclose(F);
}
