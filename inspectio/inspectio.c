#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include<stdarg.h> // va_arg
 
typedef int     (*orig_open_f_type)(const char *pathname, int flags);
typedef int     (*orig_close_f_type)(int fd);

typedef int     (*orig_dup3_f_type)(int oldfd,int newfd,int flags);
typedef int     (*orig_dup2_f_type)(int oldfd,int newfd);
typedef int     (*orig_dup_f_type)(int oldfd); 

typedef size_t (*orig_read_f_type)(int fd, void *buf, size_t count);
typedef size_t (*orig_write_f_type)(int fd, void *buf, size_t count);

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

long   tab_fd_to_writecall[1024];

long   tab_fd_to_writecall4k[1024];
long   tab_fd_to_writecall8k[1024];
long   tab_fd_to_writecall16k[1024];
long   tab_fd_to_writecall32k[1024];
long   tab_fd_to_writecall64k[1024];
long   tab_fd_to_writecall128k[1024];
long   tab_fd_to_writecall256k[1024];
long   tab_fd_to_writecall512k[1024];
long   tab_fd_to_writecall1024k[1024];
long   tab_fd_to_writecall2048k[1024];
long   tab_fd_to_writecall4096k[1024];



long  tab_fd_to_writecall[1024];



char * inspectio_str="__inspect_io__";


void printf_info_stdout(int fd,int retcode)
{
  //printf("%s close(%d)=%d (%s)\n",inspectio_str,fd,retcode,tab_fd_to_name[fd]);
  fprintf(stderr,"%s close(%d)=%d rcall=%ld wcall=%ld %s\n",inspectio_str,fd,retcode,tab_fd_to_readcall[fd],tab_fd_to_writecall[fd],tab_fd_to_name[fd]);
  fprintf(stderr,"%s close readreport  4k=%8.1ld 8k=%8.1ld 16k=%8.1ld 32k=%8.1ld 64k=%8.1ld 128k=%8.1ld 256k=%8.1ld 512k=%8.1ld 1024k=%8.1ld 2048k=%8.1ld 4096k=%8.1ld %s\n",inspectio_str,
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
	  tab_fd_to_readcall4096k[fd],
	  tab_fd_to_name[fd]
	  );  
  fprintf(stderr,"%s close writereport 4k=%8.1ld 8k=%8.1ld 16k=%8.1ld 32k=%8.1ld 64k=%8.1ld 128k=%8.1ld 256k=%8.1ld 512k=%8.1ld 1024k=%8.1ld 2048k=%8.1ld 4096k=%8.1ld %s\n",inspectio_str,
	  tab_fd_to_writecall4k[fd],
	  tab_fd_to_writecall8k[fd],
	  tab_fd_to_writecall16k[fd],
	  tab_fd_to_writecall32k[fd],
	  tab_fd_to_writecall64k[fd],
	  tab_fd_to_writecall128k[fd],
	  tab_fd_to_writecall256k[fd],
	  tab_fd_to_writecall512k[fd],
	  tab_fd_to_writecall1024k[fd],
	  tab_fd_to_writecall2048k[fd],
	  tab_fd_to_writecall4096k[fd],
	  tab_fd_to_name[fd]
	  );  
}




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


void count_write(int count,int fd)
{
  if (count >  0     && count <= 1<<12)   tab_fd_to_writecall4k[fd]++;
  if (count >  1<<12 && count <= 1<<13)   tab_fd_to_writecall8k[fd]++;
  if (count >  1<<13 && count <= 1<<14)   tab_fd_to_writecall16k[fd]++;
  if (count >  1<<14 && count <= 1<<15)   tab_fd_to_writecall32k[fd]++;
  if (count >  1<<15 && count <= 1<<16)   tab_fd_to_writecall64k[fd]++;
  if (count >  1<<16 && count <= 1<<17)   tab_fd_to_writecall128k[fd]++;
  if (count >  1<<17 && count <= 1<<18)   tab_fd_to_writecall256k[fd]++;
  if (count >  1<<18 && count <= 1<<19)   tab_fd_to_writecall512k[fd]++;
  if (count >  1<<19 && count <= 1<<20)   tab_fd_to_writecall1024k[fd]++;
  if (count >  1<<20 && count <= 1<<21)   tab_fd_to_writecall2048k[fd]++;
  if (count >  1<<21 && count <= 1<<22)   tab_fd_to_writecall4096k[fd]++;
}

size_t read(int fd, void *buf, size_t count)
{
  int size;
  int size_t_count=count;
  orig_read_f_type orig_read;
  orig_read  = (void*)dlsym(RTLD_NEXT,"read");
  size=orig_read(fd,buf,count);
  if (size>=0)
    {
      //fprintf(stderr,"%s read(%d,,%d)=%d\n",inspectio_str,fd,size_t_count,size);      
      tab_fd_to_readcall[fd]++;
      count_read(count,fd);
    }
  return size;
}


size_t write(int fd, void *buf, size_t count)
{
  int size;
  int size_t_count=count;
  orig_write_f_type orig_write;
  orig_write  = (void*)dlsym(RTLD_NEXT,"write");
  size=orig_write(fd,buf,count);
  if (size>=0)
    {
      //fprintf(stderr,"%s write(%d,,%d)=%d\n",inspectio_str,fd,size_t_count,size);      
      tab_fd_to_writecall[fd]++;
      count_write(count,fd);
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
      //fprintf(stderr,"%s fread(%d %d)=%d\n",inspectio_str,fd,count,iosize);      
      tab_fd_to_readcall[fd]++;
      count_read(count,fd);
    }
  return iosize;
}



int    open(const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;

  orig_open_f_type orig_open;
  orig_open  = (void*)dlsym(RTLD_NEXT,"open");
  
  /* if (flags & O_CREAT) */
  /*   { */
  /*     va_list arg; */
  /*     va_start(arg, flags); */
  /*     mode = va_arg(arg, int); */
  /*     va_end(arg); */
  /*     retcode=orig_open(pathname,flags,mode); */
  /*   } */
  /*   else */
  /*     { */
      retcode=orig_open(pathname,flags);      
      //}
  
  

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

  if (FP!=0)
  {
      fd=fileno(FP);
      //fprintf(stderr,"%s fopen(%s,%s)=%d\n",inspectio_str,path,mode,fd);
      strcpy(tab_fd_to_name[fd],path);
    }

  return(FP);

}



int close(int fd)
{
  int retcode;

  orig_close_f_type orig_close;
  orig_close = (void*)dlsym(RTLD_NEXT,"close");
  retcode=orig_close(fd);
  if (retcode>=0)
    {
      printf_info_stdout(fd,retcode);
    }
  return retcode;
}

int dup3(int oldfd, int newfd, int flags)
{
  int retcode;

  orig_dup3_f_type orig_dup3;
  orig_dup3 = (void*)dlsym(RTLD_NEXT,"dup3");
  retcode=orig_dup3(oldfd,newfd,flags);
  strcpy(tab_fd_to_name[newfd],tab_fd_to_name[oldfd]);
  return retcode;
}


int dup2(int oldfd, int newfd)
{
  int retcode;

  orig_dup2_f_type orig_dup2;
  orig_dup2 = (void*)dlsym(RTLD_NEXT,"dup2");
  retcode=orig_dup2(oldfd,newfd);
  strcpy(tab_fd_to_name[newfd],tab_fd_to_name[oldfd]);
  return retcode;
}

int dup(int oldfd)
{
  int retcode;

  orig_dup_f_type orig_dup;
  orig_dup = (void*)dlsym(RTLD_NEXT,"dup");
  retcode=orig_dup(oldfd);
  strcpy(tab_fd_to_name[retcode],tab_fd_to_name[oldfd]);
  return retcode;
}



