#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include<stdarg.h> // va_arg

int  stdin_copy;  
int stdout_copy;
int stderr_copy;  

FILE * FP_STDIN;
FILE * FP_STDOUT;
FILE * FP_STDERR;


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
typedef FILE * (*orig_fopen_f_type)(const char *path, const char *mode);


enum ioByFileState {
  IOBYFILE_NEW,
  IOBYFILE_OPEN,
  IOBYFILE_READ,
  IOBYFILE_WRITE,
  IOBYFILE_CLOSE,
  IOBYFILE_SEEK,
};
  

struct   ioByFileObj {

  int         state;      // 
  int         fd;         // -1 at the beginning // -2 at the end // >=0 
  long long   tab_fd_to_readcall;
  long long   tab_fd_to_writecall;
  
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
  printf("noop\n");
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
   char state[8];
   
   fprintf(FD,"printing\n"); 

   while(iter!=NULL) 
   {
     switch (iter->iobfo->state)
       {
       case IOBYFILE_NEW   :	 strcpy(state,state_new); 	 break;
       case IOBYFILE_OPEN  :	 strcpy(state,state_open); 	 break;
       case IOBYFILE_CLOSE :	 strcpy(state,state_close); 	 break;
       }
     fprintf(FD,"fd:%5.1d state:%6.6s filename:%147.147s\n",
	     iter->iobfo->fd,
	     state,
	     iter->iobfo->tab_fd_to_name);
       fprintf(FD,"writereport 4k=%8.1ld 8k=%8.1ld 16k=%8.1ld 32k=%8.1ld 64k=%8.1ld 128k=%8.1ld 256k=%8.1ld 512k=%8.1ld 1024k=%8.1ld 2048k=%8.1ld 4096k=%8.1ld %16.16s\n",
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
	       iter->iobfo->tab_fd_to_writecall4096k,
	       iter->iobfo->tab_fd_to_name
	       );
       fprintf(FD," readreport 4k=%8.1ld 8k=%8.1ld 16k=%8.1ld 32k=%8.1ld 64k=%8.1ld 128k=%8.1ld 256k=%8.1ld 512k=%8.1ld 1024k=%8.1ld 2048k=%8.1ld 4096k=%8.1ld %16.16s\n",
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
	       iter->iobfo->tab_fd_to_readcall4096k,
	       iter->iobfo->tab_fd_to_name
	       );

     
     iter=iter->next; 
   } 
 } 

struct ioByFileChainlist * prepareIoByFileChainlist()
{
  struct ioByFileChainlist * self;

  fprintf(FP_STDOUT,"prepare\n");
  self=malloc(sizeof(struct ioByFileChainlist));


  self->iobfo        = malloc(sizeof(struct ioByFileObj));

  self->iobfo->state = IOBYFILE_NEW;
  self->iobfo->fd    = -1;
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








void count_read(int count,int fd)
{
}


void count_write(int count,int fd)
{
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
      //tab_fd_to_readcall[fd]++;
      //count_read(count,fd);
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
      struct ioByFileChainlist * chainlist_elem;
      chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
      if (chainlist_elem!=NULL)
	{
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

      //fprintf(stderr,"%s write(%d,,%d)=%d\n",inspectio_str,fd,size_t_count,size);      
      //tab_fd_to_writecall[fd]++;
      //count_write(count,fd);
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
      //tab_fd_to_readcall[fd]++;
      //count_read(count,fd);
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
  orig_close = (void*)dlsym(RTLD_NEXT,"close");
  retcode=orig_close(fd);
  if (retcode>=0)
    {
      struct ioByFileChainlist * chainlist_elem;
      //fprintf(FP_STDOUT,"close : %d %d\n",chainlist_head,fd);
      chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
      if (chainlist_elem!=NULL)
	{
	  chainlist_elem->iobfo->fd=-2; 
	  chainlist_elem->iobfo->state=IOBYFILE_CLOSE;
	}
    }
  return retcode;
}

int fclose(FILE * FD)
{
  int retcode;
  int fd=fileno(FD);
  orig_fclose_f_type orig_fclose;
  orig_fclose = (void*)dlsym(RTLD_NEXT,"fclose");
  retcode=orig_fclose(FD);
  if (retcode>=0)
    {

      struct ioByFileChainlist * chainlist_elem;
      //chainlist_elem=chainlist_head->getByFd(chainlist_head,8192);
      //fprintf(FP_STDOUT,"fclose: %d %d\n",chainlist_head,fd);


      // HACK chainlist_head could be NULL for an unknown reason here...
      if (chainlist_head)
	{
	  chainlist_elem=chainlist_head->getByFd(chainlist_head,fd);
	  if (chainlist_elem!=NULL)
	    {
	      chainlist_elem->iobfo->fd=-2;
	      chainlist_elem->iobfo->state=IOBYFILE_CLOSE;
	    }
	}



      //strcpy(chainlist_elem->iobfo->tab_fd_to_name,pathname);
      //printf_info_stdout(fd,retcode);
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

  orig_dup2_f_type orig_dup2;
  orig_dup2 = (void*)dlsym(RTLD_NEXT,"dup2");
  retcode=orig_dup2(oldfd,newfd);
  //strcpy(tab_fd_to_name[newfd],tab_fd_to_name[oldfd]);
  return retcode;
}

int dup(int oldfd)
{
  int retcode;

  orig_dup_f_type orig_dup;
  orig_dup = (void*)dlsym(RTLD_NEXT,"dup");
  retcode=orig_dup(oldfd);
  //strcpy(tab_fd_to_name[retcode],tab_fd_to_name[oldfd]);
  return retcode;
}




void init() 
{ 
  //printf("Launching init()\n");

  //stdin_copy=dup(0); 
  //stdout_copy=dup(1);
  //stderr_copy=dup(2);

  //FP_STDOUT=fdopen(stdout_copy,"w+");
  //FP_STDERR=fdopen(stderr_copy,"w+");
  FP_STDOUT=fdopen(1,"w+");

  fprintf(FP_STDOUT,"Launching init()\n");
  
  chainlist_head = prepareIoByFileChainlist();
} 

void fini()  
{
  FILE * FD;
  FD=fopen("/dev/tty","w+");
  fprintf(FD,"Launching fini()\n");
  chainlist_head->printList(chainlist_head,FD);
}  



