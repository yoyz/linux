#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef int (*orig_open_f_type)(const char *pathname, int flags);

FILE * F; // log file which trace open

int open(const char *pathname, int flags, ...)
{
  fprintf(F,"open(%s,%d) \n",pathname,flags);
  orig_open_f_type orig_open;
  orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  return orig_open(pathname,flags);
}

static void con() __attribute__((constructor));

void con()
{
  pid_t pid; 
  char  log[1024];

  pid=getpid();
  sprintf(log,"/tmp/const.%d",pid);
  F=fopen(log,"w");
  fprintf(F,"I'm a constructor %d\n",pid);
}


static void des() __attribute__((destructor));

void des()
{
  pid_t pid; 
  char  log[1024];

  pid=getpid();
  sprintf(log,"/tmp/const.%d",pid);
  fprintf(F,"I'm the destructor %d\n",pid);
  fclose(F);
}
