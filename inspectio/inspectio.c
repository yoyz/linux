#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
 
typedef int     (*orig_open_f_type)(const char *pathname, int flags);
typedef int     (*orig_close_f_type)(int fd);
typedef size_t (*orig_read_f_type)(int fd, void *buf, size_t count);
//typedef ssize_t (*orig_fread_f_type)(void *ptr, size_t size, size_t nbemb, FILE *stream);
typedef size_t (*orig_fread_f_type)(void * ptr, size_t size,size_t nmemb , FILE * stream);
typedef FILE * (*orig_fopen_f_type)(const char *path, const char *mode);

/*
                    1024 file descriptor
                      V   pathname
                            V                           
*/
char tab_fd_to_name[1024][1024];

long   tab_fd_to_readcall[1024];

long   tab_fd_to_readcall4k[1024];
long   tab_fd_to_readcall8k[1024];
long   tab_fd_to_readcall16k[1024];
long   tab_fd_to_readcall32k[1024];
long   tab_fd_to_readcall64k[1024];
long   tab_fd_to_readcall128k[1024];
long   tab_fd_to_readcall256k[1024];
long   tab_fd_to_readcall512k[1024];
long   tab_fd_to_readcall1024k[1024];
long   tab_fd_to_readcall2048k[1024];
long   tab_fd_to_readcall4096k[1024];



long  tab_fd_to_writecall[1024];



char * inspectio_str="__inspect_io__";
 


void count_read(int count,int fd)
{
  if (count >  0     && count <= 1<<12)   tab_fd_to_readcall4k[fd]++;
  if (count >  1<<12 && count <= 1<<13)   tab_fd_to_readcall8k[fd]++;
  if (count >  1<<13 && count <= 1<<14)   tab_fd_to_readcall16k[fd]++;
  if (count >  1<<14 && count <= 1<<15)   tab_fd_to_readcall32k[fd]++;
  if (count >  1<<15 && count <= 1<<16)   tab_fd_to_readcall64k[fd]++;
  if (count >  1<<16 && count <= 1<<17)   tab_fd_to_readcall128k[fd]++;
  if (count >  1<<17 && count <= 1<<18)   tab_fd_to_readcall256k[fd]++;
  if (count >  1<<18 && count <= 1<<19)   tab_fd_to_readcall512k[fd]++;
  if (count >  1<<19 && count <= 1<<20)   tab_fd_to_readcall1024k[fd]++;
  if (count >  1<<20 && count <= 1<<21)   tab_fd_to_readcall2048k[fd]++;
  if (count >  1<<21 && count <= 1<<22)   tab_fd_to_readcall4096k[fd]++;
}

size_t read(int fd, void *buf, size_t count)
{
  int size;
  orig_read_f_type orig_read;
  orig_read  = (void*)dlsym(RTLD_NEXT,"read");
  size=orig_read(fd,buf,count);
  if (size>=0)
    {
      fprintf(stderr,"%s read(%d,,%d)=%d\n",inspectio_str,fd,count,size);      
      tab_fd_to_readcall[fd]++;
      count_read(count,fd);
    }
  return size;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  int iosize;
  int count=size*nmemb;
  int fd=fileno(stream);
  orig_fread_f_type orig_fread;
  orig_fread  = (void*)dlsym(RTLD_NEXT,"fread");
  iosize=orig_fread(ptr,size,nmemb,stream);
  if (iosize>=0)
    {
      fprintf(stderr,"%s fread(%d %d)=%d\n",inspectio_str,fd,count,iosize);      
      tab_fd_to_readcall[fd]++;
      count_read(count,fd);
    }
  return iosize;
}



int    open(const char *pathname, int flags,...)
{
  int retcode;
  orig_open_f_type orig_open;
  orig_open  = (void*)dlsym(RTLD_NEXT,"open");
  retcode=orig_open(pathname,flags);
  if (retcode>=0)
    {
      fprintf(stderr,"%s open(%s,%d)=%d\n",inspectio_str,pathname,flags,retcode);
      strcpy(tab_fd_to_name[retcode],pathname);
    }

  return retcode;
}

FILE *fopen(const char *path, const char *mode)
{
  FILE * FP;
  int    fd;
  orig_fopen_f_type orig_fopen;
  orig_fopen  = (void*)dlsym(RTLD_NEXT,"fopen");
  FP=orig_fopen(path,mode);
  fd=fileno(FP);
  if (fd>=0)
    {
      fprintf(stderr,"%s fopen(%s,%s)=%d\n",inspectio_str,path,mode,fd);
      strcpy(tab_fd_to_name[fd],path);
    }

  return FP;

}



int close(int fd)
{
  int retcode;

  orig_close_f_type orig_close;
  orig_close = (void*)dlsym(RTLD_NEXT,"close");
  retcode=orig_close(fd);
  if (retcode>=0)
    {
      //printf("%s close(%d)=%d (%s)\n",inspectio_str,fd,retcode,tab_fd_to_name[fd]);
      fprintf(stderr,"%s close(%d)=%d rcall=%d (%s)\n",inspectio_str,fd,retcode,tab_fd_to_readcall[fd],tab_fd_to_name[fd]);
      fprintf(stderr,"%s close readreport %s 4k=%d 8k=%d 16k=%d 32k=%d 64k=%d 128k=%d 256k=%d 512k=%d 1024k=%d 2048k=%d 4096k=%d\n",inspectio_str,tab_fd_to_name[fd],
	     tab_fd_to_readcall4k[fd],
	     tab_fd_to_readcall8k[fd],
	     tab_fd_to_readcall16k[fd],
	     tab_fd_to_readcall32k[fd],
	     tab_fd_to_readcall64k[fd],
	     tab_fd_to_readcall128k[fd],
	     tab_fd_to_readcall256k[fd],
	     tab_fd_to_readcall512k[fd],
	     tab_fd_to_readcall1024k[fd],
	     tab_fd_to_readcall2048k[fd],
	     tab_fd_to_readcall4096k[fd]
				  );

    }
  return retcode;
}


