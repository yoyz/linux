#include <string>
#include <vector>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>  //getpid
#include <sys/stat.h>
#include <sys/time.h> // gettimeofday
#include <fcntl.h>
#include <stdarg.h> // va_arg


#include <unistd.h>

#define MAX_FILEDESC 1024

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
  IOBYFILE_UNKNOW_ASSUME_CLOSED,
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




class Iaccess
{
public:
  Iaccess();
  ~Iaccess();
  long long   readcall;
  long long   writecall;

  long long   readtime_seconds;
  long long   writetime_seconds;

  long long   readtime_useconds;
  long long   writetime_useconds;

  long long   readcall4k;
  long long   readcall8k;
  long long   readcall16k;
  long long   readcall32k;
  long long   readcall64k;
  long long   readcall128k;
  long long   readcall256k;
  long long   readcall512k;
  long long   readcall1024k;
  long long   readcall2048k;
  long long   readcall4096k;  
  
  long long   writecall4k;
  long long   writecall8k;
  long long   writecall16k;
  long long   writecall32k;
  long long   writecall64k;
  long long   writecall128k;
  long long   writecall256k;
  long long   writecall512k;
  long long   writecall1024k;
  long long   writecall2048k;
  long long   writecall4096k;

};

Iaccess::Iaccess()
{  
  readcall=0;
}

Iaccess::~Iaccess()
{
}

class Ifile
{
public:
  Ifile();
  ~Ifile();
  void setFd(int newfd);
  void setName(std::string newname);
  void setState(int newstate);
  int fd;
  std::string name;
  int state;
  Iaccess iac;
};

Ifile::Ifile() : iac(Iaccess())
{
  state=IOBYFILE_UNKNOW_ASSUME_CLOSED;
  fd=-1;
  name=std::string("NULL");
}

Ifile::~Ifile()
{
}

void Ifile::setFd(int newfd)             {   fd = newfd; }
void Ifile::setState(int newstate)       {   state = newstate; }
void Ifile::setName(std::string newname) { name = newname; } 


class Iio
{
public:
  Iio();
  ~Iio();
  void noop();
  void dump();
  int existFd(int fdtoseek);
  Ifile & getFd(int fdtoseek);
  //void dump
  std::vector<Ifile> iiof;
  int a;
};

Ifile & Iio::getFd(int fdtoseek)
{
  int i;
  for (i=0;i<iiof.size();i++)
    {
      if (iiof[i].fd==fdtoseek)
	return iiof[i];
    }
  printf("ERROR Iio::getFd(%d) not found\n",fdtoseek);
  exit(1);
}

int Iio::existFd(int fdtoseek)
{
  int i=0;
  int ret=-1;
  for (i=0;i<iiof.size();i++)
    {
      if (iiof[i].fd==fdtoseek)
	return 1;
    }
  return ret;
}


void Iio::noop()
{
  a=1;
}

//Iio::Iio() : iiof(MAX_FILEDESC,Ifile())
Iio::Iio()
{
  FILE * FD;
  int    pid;
  char    logfile[1024];
  //printf("loading\n");
  orig_fopen_f_type orig_fopen;
  orig_fclose_f_type orig_fclose;

  orig_fopen   = (void*)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (void*)dlsym(RTLD_NEXT,"fclose");

  pid=getpid();
  sprintf(logfile,"/tmp/log.%d",pid);

  FD=orig_fopen(logfile,"w");  
  fprintf(FD,"Launching init()\n"); 
  orig_fclose(FD);  


}

Iio::~Iio()
{
  FILE * FD;
  pid_t pid;
  char logfile[1024];

  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;

  orig_fopen   = (void*)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (void*)dlsym(RTLD_NEXT,"fclose");
  
  pid=getpid();
  
  sprintf(logfile,"/tmp/log.%d",pid);
  dump();
  FD=orig_fopen(logfile,"a"); 
  fprintf(FD,"Launching fini()\n"); 
  //chainlist_head->printList(chainlist_head,FD); 
  orig_fclose(FD); 

}

void Iio::dump()
{
  FILE * FD;
  pid_t pid;
  char logfile[1024];
  
  int i;

  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;

  orig_fopen   = (void*)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (void*)dlsym(RTLD_NEXT,"fclose");
  
  pid=getpid();
  
  sprintf(logfile,"/tmp/log.%d",pid);

  FD=orig_fopen(logfile,"a"); 
  for (i=0;i<iiof.size();i++)
    {
      fprintf(FD,"%d %s\n",iiof[i].fd,iiof[i].name.c_str());
    }
  //chainlist_head->printList(chainlist_head,FD); 
  orig_fclose(FD); 

}

Iio myiio;

//################################################################################

void add_write_count(int fd,int count)
{
}


void add_write_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
}

void add_read_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
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

  // if (donothing)
  //   return size;

  
  if (size>=0)
    {
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
    }
  return size;
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
  
   // if (donothing)
   //   return retcode;


  if (retcode>=0)
    {
      if (myiio.existFd(retcode)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(retcode);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  myiio.iiof.push_back(ifi);
	  //myiio.getFd(retcode).setFd(retcode);
	  //my
	}
      // struct ioByFileChainlist * chainlist_elem;
      // chainlist_elem=chainlist_head->new(chainlist_head);
      // chainlist_elem->iobfo->fd=retcode;
      // chainlist_elem->iobfo->state=IOBYFILE_OPEN;
      // strcpy(chainlist_elem->iobfo->tab_fd_to_name,pathname);
    }

  return retcode;
}

//iio myiio();



static void con() __attribute__((constructor));

void con() 
{
  //printf("coucou\n");
  //Iio myiio;
  //myiio.noop();
}



static void des() __attribute__((destructor));

void des()
{
  printf("bye\n");
}
