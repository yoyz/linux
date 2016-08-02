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
typedef int     (*orig_open64_f_type)(const char *pathname, int flags,...);
typedef int     (*orig_close_f_type)(int fd);

typedef FILE *  (*orig_fopen_f_type)(const char *path, const char *mode);
typedef int     (*orig_fclose_f_type)(FILE * fd);

typedef int     (*orig_dup3_f_type)(int oldfd,int newfd,int flags);
typedef int     (*orig_dup2_f_type)(int oldfd,int newfd);
typedef int     (*orig_dup_f_type)(int oldfd); 

typedef size_t (*orig_read_f_type)(int fd, void *buf, size_t count);
typedef size_t (*orig_write_f_type)(int fd, void *buf, size_t count);

typedef size_t (*orig_fwrite_f_type)(const void *ptr, size_t size, size_t nmemb,FILE *stream);
//size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);



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
  unsigned long long   readcall;
  unsigned long long   writecall;

  unsigned long long   readtime_seconds;
  unsigned long long   writetime_seconds;

  unsigned long long   readtime_useconds;
  unsigned long long   writetime_useconds;

  unsigned long long   readcall4k;
  unsigned long long   readcall8k;
  unsigned long long   readcall16k;
  unsigned long long   readcall32k;
  unsigned long long   readcall64k;
  unsigned long long   readcall128k;
  unsigned long long   readcall256k;
  unsigned long long   readcall512k;
  unsigned long long   readcall1024k;
  unsigned long long   readcall2048k;
  unsigned long long   readcall4096k;  

  unsigned long long   readsize;

  unsigned long long   writecall4k;
  unsigned long long   writecall8k;
  unsigned long long   writecall16k;
  unsigned long long   writecall32k;
  unsigned long long   writecall64k;
  unsigned long long   writecall128k;
  unsigned long long   writecall256k;
  unsigned long long   writecall512k;
  unsigned long long   writecall1024k;
  unsigned long long   writecall2048k;
  unsigned long long   writecall4096k;
  unsigned long long   writesize;
};

Iaccess::Iaccess()
{  
   readcall=0;
   writecall=0;

   readtime_seconds=0;
   writetime_seconds=0;

   readtime_useconds=0;
   writetime_useconds=0;

   readcall4k=0;
   readcall8k=0;
   readcall16k=0;
   readcall32k=0;
   readcall64k=0;
   readcall128k=0;
   readcall256k=0;
   readcall512k=0;
   readcall1024k=0;
   readcall2048k=0;
   readcall4096k=0;  

   readsize=0;

   writecall4k=0;
   writecall8k=0;
   writecall16k=0;
   writecall32k=0;
   writecall64k=0;
   writecall128k=0;
   writecall256k=0;
   writecall512k=0;
   writecall1024k=0;
   writecall2048k=0;
   writecall4096k=0;
   writesize=0;

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
  void setOldFd(int oldfd);
  void setName(std::string newname);
  void setState(int newstate);
  int fd;
  int oldfd;
  std::string name;
  int state;
  Iaccess iac;
};

Ifile::Ifile() : iac(Iaccess())
{
  state=IOBYFILE_UNKNOW_ASSUME_CLOSED;
  fd=-2;
  oldfd=-2;
  name=std::string("NULL");
}

Ifile::~Ifile()
{
}

void Ifile::setFd(int newfd)             {   fd = newfd; }
void Ifile::setOldFd(int oldfd)          {   fd = oldfd; }
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
  printf("ERROR Iio::getFd(%d) not found\nexistFd(%d)=%d",fdtoseek,fdtoseek,existFd(fdtoseek));
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
      fprintf(FD,"[%d %d %s %d] write[%lld %lld] read[%lld %lld] \n",
	      iiof[i].fd,
	      iiof[i].oldfd,
	      iiof[i].name.c_str(),
	      iiof[i].state,
	      iiof[i].iac.writecall,
	      iiof[i].iac.writesize,
	      iiof[i].iac.readcall,
	      iiof[i].iac.readsize);
    }
  //chainlist_head->printList(chainlist_head,FD); 
  orig_fclose(FD); 

}

Iio myiio;

//################################################################################




void add_write_count(int fd,int count)
{
  //exit(1);
  if (myiio.existFd(fd)==-1)
    {
      Ifile ifi;
      ifi.setFd(fd);
      myiio.iiof.push_back(ifi);
    }
  myiio.getFd(fd).iac.writecall++;
  myiio.getFd(fd).iac.writesize+=count;
}


void add_write_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
}

void add_read_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream)
{
  int realsize=size*nmemb;
  int retsize;
  orig_fwrite_f_type orig_fwrite;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  int fd=fileno(stream);



  
  orig_fwrite  = (void*)dlsym(RTLD_NEXT,"fwrite");

  gettimeofday(&tv0,&tz);
  retsize=orig_fwrite(ptr,size,nmemb,stream);
  gettimeofday(&tv1,&tz);


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN"));
	  ifi.setState(IOBYFILE_WRITE);
	  myiio.iiof.push_back(ifi);
	}
      add_write_count(fd,retsize);
      add_write_time(fd,tv0,tv1);
    }
  return retsize;
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


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN"));
	  ifi.setState(IOBYFILE_WRITE);
	  myiio.iiof.push_back(ifi);
	}
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
    }
  return size;
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
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  myiio.iiof.push_back(ifi);
	}      
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	  myiio.getFd(fd).setFd(fd);
	  //myiio.getFd(fd).setOldFd(fd);
	}

    }

  return(FP);
}


int    open(const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;

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
      fd=retcode;
      if (myiio.existFd(retcode)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	  //myiio.getFd(fd).setFd(fd);
	  //myiio.getFd(fd).setOldFd(fd);
	}
    }

  return retcode;
}

int open64(const char *pathname, int flags,...)
{
  int retcode;
  int mode=0;
  if (flags & O_CREAT)
    { 
      va_list arg; 
      va_start(arg, flags); 
      mode = va_arg(arg, int); 
      va_end(arg); 
      retcode=open(pathname,flags,mode); 
    } 
  else
    {
      retcode=open(pathname,flags);      
    }
  return retcode;
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

  if (retcode>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setOldFd(fd);
	  ifi.setFd(-1);
	  ifi.setName(std::string("UNKNOWN"));
	  ifi.setState(IOBYFILE_CLOSE);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_CLOSE);
	  myiio.getFd(fd).setOldFd(fd);
	  myiio.getFd(fd).setFd(-1);
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

  if (retcode>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setOldFd(fd);
	  ifi.setFd(-1);
	  ifi.setName(std::string("UNKNOWN"));
	  ifi.setState(IOBYFILE_CLOSE);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_CLOSE);
	  myiio.getFd(fd).setOldFd(fd);
	  myiio.getFd(fd).setFd(-1);
	}
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
  //printf("bye\n");
}