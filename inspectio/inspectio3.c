#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>  //getpid
#include <sys/stat.h>
#include <sys/time.h> // gettimeofday
#include <fcntl.h>

#include <stdarg.h> // va_arg
//#include <unistd.h>

int  stdin_copy;  
int stdout_copy;
int stderr_copy;  

FILE * FP_STDIN;
FILE * FP_STDOUT;
FILE * FP_STDERR;

FILE * FD;

int donothing=0;

typedef void     (*orig_init_f_type)();
typedef void     (*orig_fini_f_type)();


typedef int     (*orig_open_f_type)(const char *pathname, int flags,...);
typedef int     (*orig_close_f_type)(int fd);

typedef FILE *  (*orig_fopen_f_type)(const char *path, const char *mode);
typedef int     (*orig_fclose_f_type)(FILE * fd);

typedef int     (*orig_dup3_f_type)(int oldfd,int newfd,int flags);
typedef int     (*orig_dup2_f_type)(int oldfd,int newfd);
typedef int     (*orig_dup_f_type)(int oldfd); 

typedef size_t (*orig_read_f_type)(int fd, void *buf, size_t count);
typedef size_t (*orig_write_f_type)(int fd, void *buf, size_t count);

//typedef ssize_t (*orig_fread_f_type)(void *ptr, size_t size, size_t nbemb, FILE *stream);
typedef size_t (*orig_fread_f_type)(void * ptr, size_t size,size_t nmemb , FILE * stream);
//typedef FILE * (*orig_fopen_f_type)(const char *path, const char *mode);


enum ioByFileState {
  IOBYFILE_NEW,
  IOBYFILE_OPEN,
  IOBYFILE_READ,
  IOBYFILE_WRITE,
  IOBYFILE_CLOSE,
  IOBYFILE_SEEK,
  IOBYFILE_DUP,
  IOBYFILE_DUP2,
  IOBYFILE_DUP3,
};
  

struct   ioByFileObj {

  int         state;      // 
  int         fd;         // -1 at the beginning // -2 at the end // >=0 
  long long   tab_fd_to_readcall;
  long long   tab_fd_to_writecall;

  long long   tab_fd_to_readtime_seconds;
  long long   tab_fd_to_writetime_seconds;

  long long   tab_fd_to_readtime_useconds;
  long long   tab_fd_to_writetime_useconds;

  
  long long   tab_fd_to_readcall4k;
  long long   tab_fd_to_readcall8k;
  long long   tab_fd_to_readcall16k;
  long long   tab_fd_to_readcall32k;
  long long   tab_fd_to_readcall64k;
  long long   tab_fd_to_readcall128k;
  long long   tab_fd_to_readcall256k;
  long long   tab_fd_to_readcall512k;
  long long   tab_fd_to_readcall1024k;
  long long   tab_fd_to_readcall2048k;
  long long   tab_fd_to_readcall4096k;  
  
  long long   tab_fd_to_writecall4k;
  long long   tab_fd_to_writecall8k;
  long long   tab_fd_to_writecall16k;
  long long   tab_fd_to_writecall32k;
  long long   tab_fd_to_writecall64k;
  long long   tab_fd_to_writecall128k;
  long long   tab_fd_to_writecall256k;
  long long   tab_fd_to_writecall512k;
  long long   tab_fd_to_writecall1024k;
  long long   tab_fd_to_writecall2048k;
  long long   tab_fd_to_writecall4096k;
  char        tab_fd_to_name[1024];
} iobyfile_t;


struct ioByFileChainlist
{
  struct   ioByFileObj       * iobfo;
  struct   ioByFileChainlist * next;

  void                       (*noop)(        struct ioByFileChainlist  * self);
  int                        (*size)(        struct ioByFileChainlist  * self);
  void                       (*printList)(   struct ioByFileChainlist  * self, FILE * FD);
  
  struct ioByFileChainlist * (*new)        ( struct ioByFileChainlist  * self);
  struct ioByFileChainlist * (*getByNumber)( struct ioByFileChainlist  * self,int elemNumber);
  struct ioByFileChainlist * (*getByFd)    ( struct ioByFileChainlist  * self,int fd);

  
  
};


struct ioByFileChainlist * prepareIoByFileChainlist();
struct ioByFileChainlist * chainlist_head;


char * inspectio_str="__inspect_io__";



void noopFuncIoByFileChainlist(struct ioByFileChainlist * self)
{
  //printf("noop\n");
}

int sizeFuncIoByFileChainlist(struct ioByFileChainlist * self)
{
   struct ioByFileChainlist * iter=self;
   int i=0;

   while(iter!=NULL) 
   { 
     iter=iter->next;
     i++;
   } 
   return i;
}

struct ioByFileChainlist * newFuncIoByFileChainlist(struct ioByFileChainlist * self)
{
  struct ioByFileChainlist * new;
  struct ioByFileChainlist * iter=self;
  
  new=prepareIoByFileChainlist();

  while(iter->next!=NULL) 
    { 
      iter=iter->next;
    } 
  iter->next=new;
  return new;
}

struct ioByFileChainlist * getByNumberFuncIoByFileChainlist(struct ioByFileChainlist * self,int elemNumber)
{
   struct ioByFileChainlist * iter=self;
   int i=0;
   int size=self->size(self);

   if (i>size)
     return NULL;
   
   for (i=1;i<size;i++)
     {
       iter=iter->next;
     }
   return iter;
}

struct ioByFileChainlist * getByFdFuncIoByFileChainlist(struct ioByFileChainlist * self,int fd)
{
   struct ioByFileChainlist * iter=self;
   int i=0;
   int size=self->size(self);

   if (i>size)
     return NULL;
   
   for (i=0;i<size;i++)
     {
       if (iter==NULL)
	 return NULL;
       
       if (iter->iobfo->fd==fd)
	 return iter;
       
       iter=iter->next;
     }
   return NULL;
}


void printListIoByFileChainlist(struct ioByFileChainlist * self,FILE * FD) 
 { 
   struct ioByFileChainlist * iter=self;
   char * state_new   = "new";
   char * state_open  = "open";
   char * state_read  = "read";
   char * state_write = "write";
   char * state_close = "close";
   char * state_seek =  "seek";
   char * state_dup  =  "dup";
   char * state_dup2 =  "dup2";
   char * state_dup3 =  "dup3";
   char state[8];
   
   fprintf(FD,"printing\n"); 

   while(iter!=NULL) 
   {
     switch (iter->iobfo->state)
       {
       case IOBYFILE_NEW   :	 strcpy(state,state_new); 	 break;
       case IOBYFILE_OPEN  :	 strcpy(state,state_open); 	 break;
       case IOBYFILE_CLOSE :	 strcpy(state,state_close); 	 break;
       case IOBYFILE_DUP   :	 strcpy(state,state_dup); 	 break;
       case IOBYFILE_DUP2  :	 strcpy(state,state_dup2); 	 break;
       case IOBYFILE_DUP3  :	 strcpy(state,state_dup3); 	 break;
       }
     fprintf(FD,"fd:%5.1d state:%6.6s wtime :%d.%d rtime :%d.%d filename:%115.115s\n",
	     iter->iobfo->fd,
	     state,
	     iter->iobfo->tab_fd_to_writetime_seconds,
	     iter->iobfo->tab_fd_to_writetime_useconds,
	     iter->iobfo->tab_fd_to_readtime_seconds,
	     iter->iobfo->tab_fd_to_readtime_useconds,
	     iter->iobfo->tab_fd_to_name);

     
     fprintf(FD,"writereport 4k=%8.1ld 8k=%8.1ld 16k=%8.1ld 32k=%8.1ld 64k=%8.1ld 128k=%8.1ld 256k=%8.1ld 512k=%8.1ld 1024k=%8.1ld 2048k=%8.1ld 4096k=%8.1ld \n",
	     iter->iobfo->tab_fd_to_writecall4k,
	     iter->iobfo->tab_fd_to_writecall8k,
	     iter->iobfo->tab_fd_to_writecall16k,
	     iter->iobfo->tab_fd_to_writecall32k,
	     iter->iobfo->tab_fd_to_writecall64k,
	     iter->iobfo->tab_fd_to_writecall128k,
	     iter->iobfo->tab_fd_to_writecall256k,
	     iter->iobfo->tab_fd_to_writecall512k,
	     iter->iobfo->tab_fd_to_writecall1024k,
	     iter->iobfo->tab_fd_to_writecall2048k,
	     iter->iobfo->tab_fd_to_writecall4096k
	     );
     fprintf(FD," readreport 4k=%8.1ld 8k=%8.1ld 16k=%8.1ld 32k=%8.1ld 64k=%8.1ld 128k=%8.1ld 256k=%8.1ld 512k=%8.1ld 1024k=%8.1ld 2048k=%8.1ld 4096k=%8.1ld \n",
	     iter->iobfo->tab_fd_to_readcall4k,
	     iter->iobfo->tab_fd_to_readcall8k,
	     iter->iobfo->tab_fd_to_readcall16k,
	     iter->iobfo->tab_fd_to_readcall32k,
	     iter->iobfo->tab_fd_to_readcall64k,
	     iter->iobfo->tab_fd_to_readcall128k,
	     iter->iobfo->tab_fd_to_readcall256k,
	     iter->iobfo->tab_fd_to_readcall512k,
	     iter->iobfo->tab_fd_to_readcall1024k,
	     iter->iobfo->tab_fd_to_readcall2048k,
	     iter->iobfo->tab_fd_to_readcall4096k
	     );
     
     
     iter=iter->next; 
   } 
 } 

struct ioByFileChainlist * prepareIoByFileChainlist()
{
  struct ioByFileChainlist * self;

  //fprintf(FP_STDOUT,"prepare\n");
  self=malloc(sizeof(struct ioByFileChainlist));


  self->iobfo        = malloc(sizeof(struct ioByFileObj));

  self->iobfo->state = IOBYFILE_NEW;
  self->iobfo->fd    = -1;

  self->iobfo->tab_fd_to_readtime_seconds=0;
  self->iobfo->tab_fd_to_writetime_seconds=0;

  self->iobfo->tab_fd_to_readtime_useconds=0;
  self->iobfo->tab_fd_to_writetime_useconds=0;

  self->iobfo->tab_fd_to_writecall4k=0;
  self->iobfo->tab_fd_to_writecall8k=0;
  self->iobfo->tab_fd_to_writecall16k=0;
  self->iobfo->tab_fd_to_writecall32k=0;
  self->iobfo->tab_fd_to_writecall64k=0;
  self->iobfo->tab_fd_to_writecall128k=0;
  self->iobfo->tab_fd_to_writecall256k=0;
  self->iobfo->tab_fd_to_writecall512k=0;
  self->iobfo->tab_fd_to_writecall1024k=0;
  self->iobfo->tab_fd_to_writecall2048k=0;
  self->iobfo->tab_fd_to_writecall4096k=0;

  self->iobfo->tab_fd_to_readcall4k=0;
  self->iobfo->tab_fd_to_readcall8k=0;
  self->iobfo->tab_fd_to_readcall16k=0;
  self->iobfo->tab_fd_to_readcall32k=0;
  self->iobfo->tab_fd_to_readcall64k=0;
  self->iobfo->tab_fd_to_readcall128k=0;
  self->iobfo->tab_fd_to_readcall256k=0;
  self->iobfo->tab_fd_to_readcall512k=0;
  self->iobfo->tab_fd_to_readcall1024k=0;
  self->iobfo->tab_fd_to_readcall2048k=0;
  self->iobfo->tab_fd_to_readcall4096k=0;

  strcpy(self->iobfo->tab_fd_to_name,"<unknown>");
  
  self->next         = NULL;
  self->noop         = noopFuncIoByFileChainlist;
  self->printList    = printListIoByFileChainlist;
  self->size         = sizeFuncIoByFileChainlist;
  self->getByNumber  = getByNumberFuncIoByFileChainlist;
  self->getByFd      = getByFdFuncIoByFileChainlist;
  self->new          = newFuncIoByFileChainlist;
  
  return(self);
}



void add_write_time(int fd,  struct timeval tv0,  struct timeval tv1)
{

  struct ioByFileChainlist * chainlist_elem;
  chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
  if (chainlist_elem==NULL)
    {
      chainlist_elem=chainlist_head->new(chainlist_head);
      chainlist_elem->iobfo->fd=fd;
      chainlist_elem->iobfo->state=IOBYFILE_OPEN;
    }
  
  
  chainlist_elem->iobfo->tab_fd_to_writetime_seconds=chainlist_elem->iobfo->tab_fd_to_writetime_seconds   + ( tv1.tv_sec  - tv0.tv_sec  );

  chainlist_elem->iobfo->tab_fd_to_writetime_useconds=chainlist_elem->iobfo->tab_fd_to_writetime_useconds + ( tv1.tv_usec - tv0.tv_usec );
  
  if (chainlist_elem->iobfo->tab_fd_to_writetime_useconds>=1000*1000)
    {
      chainlist_elem->iobfo->tab_fd_to_writetime_seconds  = chainlist_elem->iobfo->tab_fd_to_writetime_seconds+(chainlist_elem->iobfo->tab_fd_to_writetime_useconds/(1000*1000));
      chainlist_elem->iobfo->tab_fd_to_writetime_useconds = chainlist_elem->iobfo->tab_fd_to_writetime_useconds-(1000*1000);
    }

  if (chainlist_elem->iobfo->tab_fd_to_writetime_useconds<0)
    {
      chainlist_elem->iobfo->tab_fd_to_writetime_seconds  =       chainlist_elem->iobfo->tab_fd_to_writetime_seconds  - 1;
      chainlist_elem->iobfo->tab_fd_to_writetime_useconds =       chainlist_elem->iobfo->tab_fd_to_writetime_useconds + 1000*1000;
    }
  
}


void add_read_time(int fd,  struct timeval tv0,  struct timeval tv1)
{

  struct ioByFileChainlist * chainlist_elem;
  chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
  if (chainlist_elem==NULL)
    {
      chainlist_elem=chainlist_head->new(chainlist_head);
      chainlist_elem->iobfo->fd=fd;
      chainlist_elem->iobfo->state=IOBYFILE_OPEN;
    }
  
  chainlist_elem->iobfo->tab_fd_to_readtime_seconds=chainlist_elem->iobfo->tab_fd_to_readtime_seconds   + ( tv1.tv_sec  - tv0.tv_sec  );

  chainlist_elem->iobfo->tab_fd_to_readtime_useconds=chainlist_elem->iobfo->tab_fd_to_readtime_useconds + ( tv1.tv_usec - tv0.tv_usec );
  
  if (chainlist_elem->iobfo->tab_fd_to_readtime_useconds>=1000*1000)
    {
      chainlist_elem->iobfo->tab_fd_to_readtime_seconds  = chainlist_elem->iobfo->tab_fd_to_readtime_seconds+(chainlist_elem->iobfo->tab_fd_to_readtime_useconds/(1000*1000));
      chainlist_elem->iobfo->tab_fd_to_readtime_useconds = chainlist_elem->iobfo->tab_fd_to_readtime_useconds-(1000*1000);
    }

  if (chainlist_elem->iobfo->tab_fd_to_readtime_useconds<0)
    {
      chainlist_elem->iobfo->tab_fd_to_readtime_seconds  =       chainlist_elem->iobfo->tab_fd_to_readtime_seconds  - 1;
      chainlist_elem->iobfo->tab_fd_to_readtime_useconds =       chainlist_elem->iobfo->tab_fd_to_readtime_useconds + 1000*1000;
    }
}




void count_read(int count,int fd)
{
}


void count_write(int count,int fd)
{
}



void add_write_count(int fd,int count)
{
  struct ioByFileChainlist * chainlist_elem;
  chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
  if (chainlist_elem==NULL)
    {
      chainlist_elem=chainlist_head->new(chainlist_head);
      chainlist_elem->iobfo->fd=fd;
      chainlist_elem->iobfo->state=IOBYFILE_WRITE;
    }
  
  if (count >  0     && count <= 1<<12)   chainlist_elem->iobfo->tab_fd_to_writecall4k++;
  if (count >  1<<12 && count <= 1<<13)   chainlist_elem->iobfo->tab_fd_to_writecall8k++;
  if (count >  1<<13 && count <= 1<<14)   chainlist_elem->iobfo->tab_fd_to_writecall16k++;
  if (count >  1<<14 && count <= 1<<15)   chainlist_elem->iobfo->tab_fd_to_writecall32k++;
  if (count >  1<<15 && count <= 1<<16)   chainlist_elem->iobfo->tab_fd_to_writecall64k++;
  if (count >  1<<16 && count <= 1<<17)   chainlist_elem->iobfo->tab_fd_to_writecall128k++;
  if (count >  1<<17 && count <= 1<<18)   chainlist_elem->iobfo->tab_fd_to_writecall256k++;
  if (count >  1<<18 && count <= 1<<19)   chainlist_elem->iobfo->tab_fd_to_writecall512k++;
  if (count >  1<<19 && count <= 1<<20)   chainlist_elem->iobfo->tab_fd_to_writecall1024k++;
  if (count >  1<<20 && count <= 1<<21)   chainlist_elem->iobfo->tab_fd_to_writecall2048k++;
  if (count >  1<<21 && count <= 1<<22)   chainlist_elem->iobfo->tab_fd_to_writecall4096k++;
  
}

void add_read_count(int fd,int count)
{
  struct ioByFileChainlist * chainlist_elem;
  chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
  if (chainlist_elem==NULL)
    {
      chainlist_elem=chainlist_head->new(chainlist_head);
      chainlist_elem->iobfo->fd=fd;
      chainlist_elem->iobfo->state=IOBYFILE_READ;
    }
  
  if (count >  0     && count <= 1<<12)   chainlist_elem->iobfo->tab_fd_to_readcall4k++;
  if (count >  1<<12 && count <= 1<<13)   chainlist_elem->iobfo->tab_fd_to_readcall8k++;
  if (count >  1<<13 && count <= 1<<14)   chainlist_elem->iobfo->tab_fd_to_readcall16k++;
  if (count >  1<<14 && count <= 1<<15)   chainlist_elem->iobfo->tab_fd_to_readcall32k++;
  if (count >  1<<15 && count <= 1<<16)   chainlist_elem->iobfo->tab_fd_to_readcall64k++;
  if (count >  1<<16 && count <= 1<<17)   chainlist_elem->iobfo->tab_fd_to_readcall128k++;
  if (count >  1<<17 && count <= 1<<18)   chainlist_elem->iobfo->tab_fd_to_readcall256k++;
  if (count >  1<<18 && count <= 1<<19)   chainlist_elem->iobfo->tab_fd_to_readcall512k++;
  if (count >  1<<19 && count <= 1<<20)   chainlist_elem->iobfo->tab_fd_to_readcall1024k++;
  if (count >  1<<20 && count <= 1<<21)   chainlist_elem->iobfo->tab_fd_to_readcall2048k++;
  if (count >  1<<21 && count <= 1<<22)   chainlist_elem->iobfo->tab_fd_to_readcall4096k++;
  
}


size_t read(int fd, void *buf, size_t count)
{
  int size;
  int size_t_count=count;
  orig_read_f_type orig_read;
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  
  orig_read  = (void*)dlsym(RTLD_NEXT,"read");

  gettimeofday(&tv0,&tz);
  size=orig_read(fd,buf,count);
  gettimeofday(&tv1,&tz);

  if (donothing)
    return size;
  
  if (size>=0)
    {
      add_read_count(fd,size);
      add_read_time(fd,tv0,tv1);
    }      

  return size;
}


size_t write(int fd, void *buf, size_t count)
{
  int size;
  int size_t_count=count;
  orig_write_f_type orig_write;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;


  
  orig_write  = (void*)dlsym(RTLD_NEXT,"write");

  gettimeofday(&tv0,&tz);
  size=orig_write(fd,buf,count);
  gettimeofday(&tv1,&tz);

  if (donothing)
    return size;

  
  if (size>=0)
    {
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
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

  if (donothing)
    return iosize;

  
  if (iosize>=0)
    {
      //add_read_time
    }
  return iosize;
}



int    open(const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;

  orig_open_f_type orig_open;
  orig_open  = (void*)dlsym(RTLD_NEXT,"open");
  
   if (flags & O_CREAT)
     { 
       va_list arg; 
       va_start(arg, flags); 
       mode = va_arg(arg, int); 
       va_end(arg); 
       retcode=orig_open(pathname,flags,mode); 
     } 
     else
       {
       retcode=orig_open(pathname,flags);      
     }
  
   if (donothing)
     return retcode;


  if (retcode>=0)
    {
      struct ioByFileChainlist * chainlist_elem;
      chainlist_elem=chainlist_head->new(chainlist_head);
      chainlist_elem->iobfo->fd=retcode;
      chainlist_elem->iobfo->state=IOBYFILE_OPEN;
      strcpy(chainlist_elem->iobfo->tab_fd_to_name,pathname);
    }

  return retcode;
}

FILE *fopen(const char *pathname, const char *mode)
{
  FILE * FP;
  int    fd;
  orig_fopen_f_type orig_fopen;
  orig_fopen  = (void*)dlsym(RTLD_NEXT,"fopen");
  FP=orig_fopen(pathname,mode);

  if (donothing)
    return FP;

  
  if (FP!=0)
  {
      fd=fileno(FP);
      struct ioByFileChainlist * chainlist_elem;
      chainlist_elem=chainlist_head->new(chainlist_head);
      chainlist_elem->iobfo->fd=fd;
      chainlist_elem->iobfo->state=IOBYFILE_OPEN;
      strcpy(chainlist_elem->iobfo->tab_fd_to_name,pathname);

      //fprintf(stderr,"%s fopen(%s,%s)=%d\n",inspectio_str,path,mode,fd);
      //strcpy(tab_fd_to_name[fd],path);
    }

  return(FP);

}



int close(int fd)
{
  int retcode;

  orig_close_f_type orig_close;

  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;

  
  orig_close = (void*)dlsym(RTLD_NEXT,"close");

  gettimeofday(&tv0,&tz);
  retcode=orig_close(fd);
  gettimeofday(&tv1,&tz);

  if (donothing)
    return retcode;
  
  if (retcode>=0)
    {
      struct ioByFileChainlist * chainlist_elem;
      //fprintf(FP_STDOUT,"close : %d %d\n",chainlist_head,fd);
      chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
      if (chainlist_elem!=NULL)
	{
	  chainlist_elem->iobfo->fd=-2; 
	  chainlist_elem->iobfo->state=IOBYFILE_CLOSE;
	  add_write_time(fd,tv0,tv1);
	}
    }
  return retcode;
}

int fclose(FILE * FD)
{
  int retcode;
  int fd=fileno(FD);

  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  
  orig_fclose_f_type orig_fclose;
  orig_fclose = (void*)dlsym(RTLD_NEXT,"fclose");

  gettimeofday(&tv0,&tz);
  retcode=orig_fclose(FD);
  gettimeofday(&tv1,&tz);

  if (donothing)
    return retcode;

  
  if (retcode>=0)
    {
      struct ioByFileChainlist * chainlist_elem;
      chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
      if (chainlist_elem!=NULL)
	{
	  chainlist_elem->iobfo->fd=-2; 
	  chainlist_elem->iobfo->state=IOBYFILE_CLOSE;
	  add_write_time(fd,tv0,tv1);
	}

    }
  return retcode;
}



int dup3(int oldfd, int newfd, int flags)
{
  int retcode;

  orig_dup3_f_type orig_dup3;
  orig_dup3 = (void*)dlsym(RTLD_NEXT,"dup3");
  retcode=orig_dup3(oldfd,newfd,flags);
  //strcpy(tab_fd_to_name[newfd],tab_fd_to_name[oldfd]);
  return retcode;
}


int dup2(int oldfd, int newfd)
{
  int retcode;
  struct ioByFileChainlist * chainlist_elem_orig;
  struct ioByFileChainlist * chainlist_elem_dest;

  orig_dup2_f_type orig_dup2;
  orig_dup2 = (void*)dlsym(RTLD_NEXT,"dup2");
  retcode=orig_dup2(oldfd,newfd);

  if (retcode>=0)
    {
      // Tracking the newfd AND IF it exist close it
      // dup2 do it silently for you
      chainlist_elem_dest=chainlist_head->getByFd(chainlist_head,newfd);
      if (chainlist_elem_dest!=NULL)
	{
	  chainlist_elem_dest->iobfo->state=IOBYFILE_CLOSE;
	  chainlist_elem_dest->iobfo->fd=-2;
	}
      chainlist_elem_dest=NULL;
      
      // Tracking the oldfd and create it, if it doesn't exist
      chainlist_elem_orig=chainlist_head->getByFd(chainlist_head,oldfd);
      if (chainlist_elem_orig==NULL)
	{
	  chainlist_elem_orig=chainlist_head->new(chainlist_head);
	  chainlist_elem_orig->iobfo->fd=oldfd;
	  chainlist_elem_orig->iobfo->state=IOBYFILE_OPEN;
	}

      // 
      // Create a newfd
      // and copy the filename from the oldfd to newfd
      chainlist_elem_dest=chainlist_head->new(chainlist_head);
      chainlist_elem_dest->iobfo->fd=newfd;
      chainlist_elem_dest->iobfo->state=IOBYFILE_DUP2;
      strcpy(chainlist_elem_dest->iobfo->tab_fd_to_name,chainlist_elem_orig->iobfo->tab_fd_to_name);
    }
 
  return retcode;
}

int dup(int oldfd)
{
  int retcode;
  struct ioByFileChainlist * chainlist_elem_orig;
  struct ioByFileChainlist * chainlist_elem_dest;
  
  orig_dup_f_type orig_dup;
  orig_dup = (void*)dlsym(RTLD_NEXT,"dup");
  retcode=orig_dup(oldfd);

  if (retcode>=0)
    {
      chainlist_elem_orig=chainlist_head->getByFd(chainlist_head,oldfd);
      if (chainlist_elem_orig==NULL)
	{
	  chainlist_elem_orig=chainlist_head->new(chainlist_head);
	  chainlist_elem_orig->iobfo->fd=oldfd;
	  chainlist_elem_orig->iobfo->state=IOBYFILE_OPEN;
	}
      
      chainlist_elem_dest=chainlist_head->new(chainlist_head);
      chainlist_elem_dest->iobfo->fd=retcode;
      chainlist_elem_dest->iobfo->state=IOBYFILE_DUP;
      strcpy(chainlist_elem_dest->iobfo->tab_fd_to_name,chainlist_elem_orig->iobfo->tab_fd_to_name);
    }
  
  
  //strcpy(tab_fd_to_name[retcode],tab_fd_to_name[oldfd]);
  return retcode;
}



static void con() __attribute__((constructor));

void con() 
{
  //FILE *  FD;
  pid_t   pid;
  char    logfile[1024];

  orig_fopen_f_type orig_fopen;
  orig_fclose_f_type orig_fclose;

  orig_fopen   = (void*)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (void*)dlsym(RTLD_NEXT,"fclose");

  //FP=orig_fopen(pathname,mode);
  
  pid=getpid();
  sprintf(logfile,"/tmp/log.%d",pid);

  FD=orig_fopen(logfile,"w");  
  fprintf(FD,"Launching init()\n"); 
  orig_fclose(FD);  
  
  chainlist_head = prepareIoByFileChainlist();
} 

static void des() __attribute__((destructor));

void des()
{
  //return;
  pid_t pid;
  char logfile[1024];

  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;

  orig_fopen   = (void*)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (void*)dlsym(RTLD_NEXT,"fclose");
  
  pid=getpid();
  
  sprintf(logfile,"/tmp/log.%d",pid);

  FD=orig_fopen(logfile,"a"); 
  fprintf(FD,"Launching fini()\n"); 
  chainlist_head->printList(chainlist_head,FD); 
  orig_fclose(FD); 
}  
