#include <iostream>
#include <dlfcn.h>          
#include <stdio.h>
#include <stdlib.h>


//#define MAXBUF 1024*1024*1024*3
typedef FILE *  (*orig_fopen_f_type)(const char *path, const char *mode);
char * buffer_setvbuff=NULL;


FILE *fopen(const char *pathname, const char *mode)
{
  FILE * FP;
  int    fd;
  size_t maxbuf=(size_t)1024*1024*1024*4;

  orig_fopen_f_type orig_fopen;
  std::string str(pathname);
  orig_fopen  = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  if (str.find("SRME3D_GRID_FILE")!=-1)
  {
	printf("catch fopen\n");
  	FP=orig_fopen(pathname,mode);
	if (buffer_setvbuff==NULL)
		buffer_setvbuff=(char*)malloc(sizeof(char)*maxbuf);
  	setvbuf(FP,buffer_setvbuff, _IOFBF ,maxbuf);
  }
  else
	FP=orig_fopen(pathname,mode);

  return(FP);
}

