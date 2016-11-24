#include <string>
#include <string.h> // memset
#include <sstream>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>  //getpid
#include <sys/stat.h>
#include <sys/time.h> // gettimeofday
#include <fcntl.h>    // openat
#include <stdarg.h> // va_arg
#include <mutex>          // std::mutex

#include <unistd.h>
#include <dirent.h>  // opendir/closedir
#define MAX_FILEDESC 1024

std::mutex mtx;  

std::string getStrFDInfo( long fd );

typedef void     (*orig_init_f_type)();
typedef void     (*orig_fini_f_type)();


typedef int     (*orig_open_f_type)(const char *pathname, int flags,...);
typedef int     (*orig_openat_f_type  )(int dirfd,const char *pathname, int flags,...);
typedef int     (*orig_openat64_f_type)(int dirfd,const char *pathname, int flags,...);
typedef int     (*orig_open64_f_type)(const char *pathname, int flags,...);
typedef FILE *  (*orig_fopen_f_type)(const char *path, const char *mode);
typedef FILE *  (*orig_fdopen_f_type)(int fd, const char *mode);

typedef int     (*orig_fclose_f_type)(FILE * fd);
typedef int     (*orig_close_f_type)(int fd);

typedef int     (*orig_dup3_f_type)(int oldfd,int newfd,int flags);
typedef int     (*orig_dup2_f_type)(int oldfd,int newfd);
typedef int     (*orig_dup_f_type)(int oldfd); 

typedef ssize_t (*orig_read_f_type)(int fd, void *buf, size_t count);
typedef size_t  (*orig_fread_f_type)(const void *ptr, size_t size, size_t nmemb,FILE *stream);


typedef ssize_t (*orig_write_f_type)(int fd, const void *buf, size_t count);
typedef ssize_t (*orig_write64_f_type)(int fd, const void *buf, size_t count);
typedef ssize_t (*orig_writev_f_type)(int fd, const struct iovec *iov, int iovcnt);
typedef ssize_t (*orig_pwrite_f_type)(int fd, const void *buf, size_t count, off_t offset);
typedef ssize_t (*orig_pwrite64_f_type)(int fd, const void *buf, size_t count, off_t offset);
typedef ssize_t (*orig_pwritev_f_type)(int fd, const struct iovec *iov, int iovcnt,off_t offset);
typedef size_t  (*orig_fwrite_f_type)(const void *ptr, size_t size, size_t nmemb,FILE *stream);



typedef int     (*orig_fprintf_f_type)(FILE *stream, const char *format, ...);
//size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);



//typedef ssize_t (*orig_fread_f_type)(void *ptr, size_t size, size_t nbemb, FILE *stream);

//typedef FILE * (*orig_fopen_f_type)(const char *path, const char *mode);

enum ioByFileState
  {
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

enum iioStatus
  {
    IIO_BEGIN,
    IIO_RUNNING,
    IIO_ENDING,
  };

class Logger
{
public:
  Logger();
  void add(std::string str);
  std::vector<std::string> logstr;
};

//Logger::Logger() : logstr(1,std::string())
Logger::Logger() 
{
  
}

void Logger::add(std::string str)
{
  logstr.push_back(str);
}

Logger  log;

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
  std::string dump();
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

void Ifile::setFd(int fdparam)                {      fd = fdparam; }
void Ifile::setOldFd(int fdparam)             {   oldfd = fdparam; }
void Ifile::setState(int newstate)       {   state = newstate; }
void Ifile::setName(std::string newname) { name = newname; }


std::string Ifile::dump()
{
  char str[2048];

  //std::ostringstream oss;
  //std::string 
  sprintf(str,"WRITE[%8lld %8lld] READ[%8lld %8lld] [%d %d %s %d] \n",
	      iac.writecall,
	      iac.writesize,
	      iac.readcall,
	      iac.readsize,
	      fd,
	      oldfd,
	      name.c_str(),
	      state
	      );
  /*
  oss
    << "["
    << iac.writecall    << " " 
    << iac.writesize    << ""  
    <<  "] ["           << ""
    << iac.readcall     << " " 
    << iac.readsize     << " " 
    << "]   ["          << ""
    << fd               << " "
    << oldfd            << " "
    << name.c_str()     << " "
    << state            << "]\n";
  */
  return std::string(str);
  //return oss.str();
}


class Iio
{
public:
  Iio();
  ~Iio();
  void noop();
  void dump();
  int existFd(int fdtoseek);
  Ifile & getFd(int fdtoseek);
  int getStatus() { return status; }
  //void dump
  std::vector<Ifile> iiof;
  int a;
  int status; // 0=begin 1=running 2=end
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
  int    pid=1;
  char    logfile[1024];
  DIR   * procselffd;
  struct dirent *myfile_in_procselfd;
  struct stat mystat;
  std::string strStarDot(".");
  std::string strStarDotdot("..");

  //printf("loading\n");
  orig_fopen_f_type orig_fopen;
  orig_fclose_f_type orig_fclose;
  status=IIO_BEGIN;
  mtx.lock();
  orig_fopen   = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");

  pid=getpid();
  sprintf(logfile,"/tmp/log.%d",pid);
  FD=orig_fopen(logfile,"w");    
  
  // open the /proc/self/fd/ and find the filename and open descriptor of the current process
  // to understand what is the current state of the file descriptor of this process  
  procselffd = opendir("/proc/self/fd");
  while((myfile_in_procselfd = readdir(procselffd)) != NULL)
    {
      Ifile ifi;
      std::string str_myfile_in_procselfd(myfile_in_procselfd->d_name);

      // we don't want . and .. , just the file 0, 1, 2, etc...
      if (str_myfile_in_procselfd != strStarDot &&
	  str_myfile_in_procselfd != strStarDotdot)
	{
	  ifi.setFd(atoi(str_myfile_in_procselfd.c_str()));
	  ifi.setName(getStrFDInfo(atoi(str_myfile_in_procselfd.c_str())));
	  ifi.setState(IOBYFILE_OPEN);
	  iiof.push_back(ifi);
	  //stat(myfile_in_procselfd->d_name, &mystat);

	  fprintf(FD,"%s %s\n",myfile_in_procselfd->d_name,getStrFDInfo(atoi(str_myfile_in_procselfd.c_str())).c_str());
	}
      
      
      
      //ifi.setName(getStrFDInfo(atoi(
    }
  closedir(procselffd);


  fprintf(FD,"Launching init()\n");
  orig_fclose(FD);
  status=IIO_RUNNING;
  mtx.unlock();

}

Iio::~Iio()
{
  FILE * FD;
  pid_t pid=1;
  char logfile[1024];

  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;
  status=IIO_ENDING;
  //mtx.lock();
  orig_fopen   = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");
  
  pid=getpid();
  
  sprintf(logfile,"/tmp/log.%d",pid);
  dump();
  FD=orig_fopen(logfile,"a");
  
  fprintf(FD,"Launching fini()\n"); 
  //chainlist_head->printList(chainlist_head,FD); 
  orig_fclose(FD); 
  ////mtx.unlock();
}




void Iio::dump()
{
  FILE * FD;
  pid_t pid=1;
  char logfile[1024];
  //int pid=1;
  int i;

  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;



  orig_fopen   = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");
  
  pid=getpid();
  
  sprintf(logfile,"/tmp/log.%d",pid);

  FD=orig_fopen(logfile,"a"); 
  for (i=0;i<iiof.size();i++)
    {
      /*
      fprintf(FD,"write[%lld %lld] read[%lld %lld] [%d %d %s %d] \n",
	      iiof[i].iac.writecall,
	      iiof[i].iac.writesize,
	      iiof[i].iac.readcall,
	      iiof[i].iac.readsize,
	      iiof[i].fd,
	      iiof[i].oldfd,
	      iiof[i].name.c_str(),
	      iiof[i].state
	      );
      */
      fprintf(FD,iiof[i].dump().c_str());
    }
  for (i=0;i<log.logstr.size();i++)
    {
      if (i==0)
	fprintf(FD,"LOG\n");
      fprintf(FD,log.logstr[i].c_str());
    }

  //chainlist_head->printList(chainlist_head,FD); 
  orig_fclose(FD); 
}

//################################################################################

Iio     myiio;

//################################################################################

std::string getStrFDInfo( long fd )
{
  int size=1024;
  char buf[1024];
  std::string str; 
  char path[1024];
  
  sprintf( path, "/proc/self/fd/%d", fd );
 
  memset( &buf[0], 0, size );
  ssize_t s = readlink( path, &buf[0], size );
  str=std::string(buf);

  return str;
}


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

void add_read_count(int fd,int count)
{
  //exit(1);
  if (myiio.existFd(fd)==-1)
    {
      Ifile ifi;
      ifi.setFd(fd);
      myiio.iiof.push_back(ifi);
    }
  myiio.getFd(fd).iac.readcall++;
  myiio.getFd(fd).iac.readsize+=count;
}


void add_write_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
}

void add_read_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
}
/*
int fprintf(FILE *stream, const char *format, ...)
{
  int retcode;
  int fd;

  orig_fprintf_f_type orig_fprintf;
  orig_fprintf  = (orig_fprintf_f_type)dlsym(RTLD_NEXT,"fprintf");

  va_list args;
  va_start(args, format);
  retcode=orig_fprintf(stream, format, args);
  va_end(args);
  return retcode;
}
*/

size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream)
{
  int realsize=size*nmemb;
  int retsize;
  orig_fwrite_f_type orig_fwrite;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  int fd;
  //mtx.lock();
  

  fd=fileno(stream);


  orig_fwrite  = (orig_fwrite_f_type)dlsym(RTLD_NEXT,"fwrite");

  gettimeofday(&tv0,&tz);
  retsize=orig_fwrite(ptr,size,nmemb,stream);
  gettimeofday(&tv1,&tz);

  if (myiio.getStatus()!=IIO_RUNNING) { return retsize; }
  
  std::string str;
  std::ostringstream oss;
  oss << "fwrite(" << ptr << "(" << fd << ")" << "," << size << "," << nmemb << "," << stream << ")="<< retsize << "\n"; 
  log.add(oss.str());


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  //ifi.setName(std::string("UNKNOWN-FWRITE"));
	  ifi.setName(getStrFDInfo(fd));
	  ifi.setState(IOBYFILE_WRITE);
	  myiio.iiof.push_back(ifi);
	}
      add_write_count(fd,retsize);
      add_write_time(fd,tv0,tv1);
    }
  //mtx.unlock();
  return retsize;
}

size_t fread(void *ptr, size_t size, size_t nmemb,FILE *stream)
{
  int realsize=size*nmemb;
  int retsize;
  orig_fwrite_f_type orig_fread;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  int fd;
  //mtx.lock();
  

  fd=fileno(stream);


  orig_fread  = (orig_fread_f_type)dlsym(RTLD_NEXT,"fread");

  gettimeofday(&tv0,&tz);
  retsize=orig_fread(ptr,size,nmemb,stream);
  gettimeofday(&tv1,&tz);

  std::string str;
  std::ostringstream oss;
  oss << "fread(" << ptr << "(" << fd << ")" << "," << size << "," << nmemb << "," << stream << ")="<< retsize << "\n"; 
  log.add(oss.str());


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  //ifi.setName(std::string("UNKNOWN-FWRITE"));
	  ifi.setName(getStrFDInfo(fd));
	  ifi.setState(IOBYFILE_WRITE);
	  myiio.iiof.push_back(ifi);
	}
      add_read_count(fd,retsize);
      add_read_time(fd,tv0,tv1);
    }
  //mtx.unlock();
  return retsize;
}



size_t fwrite64(const void *ptr, size_t size, size_t nmemb,FILE *stream)
{  
  return fwrite(ptr,size,nmemb,stream);
}

ssize_t write(int fd, const void *buf, size_t count)
{
  int size;
  int size_t_count=count;
  orig_write_f_type orig_write;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;

  //mtx.lock(); 
  orig_write  = (orig_write_f_type)dlsym(RTLD_NEXT,"write");

  gettimeofday(&tv0,&tz);
  size=orig_write(fd,buf,count);
  gettimeofday(&tv1,&tz);

  std::string str;
  std::ostringstream oss;
  oss << "write(" << fd << "," << buf << "," << count << ")\n"; 
  log.add(oss.str());


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(getStrFDInfo(fd));
	  //ifi.setName(std::string("UNKNOWN-WRITE"));
	  ifi.setState(IOBYFILE_WRITE);
	  myiio.iiof.push_back(ifi);
	}
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
    }
  //mtx.unlock();
  return size;   
}

ssize_t read(int fd, void *buf, size_t count)
{
  int size;
  int size_t_count=count;
  orig_read_f_type orig_read;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;

  //mtx.lock(); 
  orig_read  = (orig_read_f_type)dlsym(RTLD_NEXT,"read");

  gettimeofday(&tv0,&tz);
  size=orig_read(fd,buf,count);
  gettimeofday(&tv1,&tz);

  std::string str;
  std::ostringstream oss;
  oss << "read(" << fd << "," << buf << "," << count << ")\n"; 
  log.add(oss.str());


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(getStrFDInfo(fd));
	  //ifi.setName(std::string("UNKNOWN-WRITE"));
	  ifi.setState(IOBYFILE_READ);
	  myiio.iiof.push_back(ifi);
	}
      add_read_count(fd,size);
      add_read_time(fd,tv0,tv1);
    }
  //mtx.unlock();
  return size;   
}


ssize_t write64(int fd, void *buf, size_t count)
{
  return write(fd,buf,count);
}


ssize_t pwrite(int fd, const void *buf, size_t count,off_t offset)
{
  int size;
  int size_t_count=count;
  orig_pwrite_f_type orig_pwrite;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  //mtx.lock();
  orig_pwrite  = (orig_pwrite_f_type)dlsym(RTLD_NEXT,"pwrite");

  gettimeofday(&tv0,&tz);
  size=orig_pwrite(fd,buf,count,offset);
  gettimeofday(&tv1,&tz);

  std::string str;
  std::ostringstream oss;
  oss << "pwrite(" << fd << "," << buf << "," << count << ")\n"; 
  log.add(oss.str());


  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN-PWRITE"));
	  ifi.setState(IOBYFILE_WRITE);
	  myiio.iiof.push_back(ifi);
	}
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
    }
  //mtx.unlock();
  return size;
}

ssize_t pwrite64(int fd, const void *buf, size_t count,off_t offset)
{
  pwrite(fd,buf,count,offset);
}



ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
  ssize_t size;
  orig_writev_f_type orig_writev;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  //mtx.lock();
  orig_writev  = (orig_writev_f_type)dlsym(RTLD_NEXT,"writev");

  
  std::string str;
  std::ostringstream oss;
  oss << "writev(" << fd << "," << iovcnt << ")\n"; 
  log.add(oss.str());

  size=orig_writev(fd,iov,iovcnt);
  //mtx.unlock();
  return size;
}

ssize_t writev64(int fd, const struct iovec *iov, int iovcnt)
{
  return writev(fd,iov,iovcnt);
}

ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt,off_t offset)
{
  int size;
  orig_pwritev_f_type orig_pwritev;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  //mtx.lock();
  orig_pwritev  = (orig_pwritev_f_type)dlsym(RTLD_NEXT,"pwritev");

  
  std::string str;
  std::ostringstream oss;
  oss << "pwritev(" << fd << "," << iovcnt << ")\n"; 
  log.add(oss.str());

  size=orig_pwritev(fd,iov,iovcnt,offset);
  //mtx.unlock();
  return size;
}

ssize_t pwritev64(int fd, const struct iovec *iov, int iovcnt,off_t offset)
{
  return pwritev(fd,iov,iovcnt,offset);
}


FILE *fopen(const char *pathname, const char *mode)
{
  FILE * FP;
  int    fd;
  orig_fopen_f_type orig_fopen;

  //mtx.lock();
  
  orig_fopen  = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
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


      std::string str;
      std::ostringstream oss;
      oss << "fopen("<<pathname<< mode << ")="<< FP<< "(" << fd << ")" << "\n"; 
      log.add(oss.str());            
  }
  //mtx.unlock();
  return(FP);
}

FILE *fopen64(const char *pathname, const char *mode)
{
  return fopen(pathname,mode);
}

FILE *fdopen(int fd, const char *mode)
{
  FILE * file;
  orig_fdopen_f_type orig_fdopen;

  //mtx.lock();
  
  orig_fdopen  = (orig_fdopen_f_type)dlsym(RTLD_NEXT,"fdopen");
  
  
  file=orig_fdopen(fd,mode);
  
  if (file!=NULL)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setState(IOBYFILE_OPEN);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	}
    }

  std::string str;
  std::ostringstream oss;
  oss << "fdopen("<<fd<< ","<< mode << ")=" << file << "\n"; 
  log.add(oss.str());
  //mtx.unlock();
  return file;  
}

FILE *fdopen64(int fd, const char *mode)
{
  return fdopen(fd,mode);
}


extern int openat(int dirfd, const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;
  orig_openat_f_type orig_openat;

  //mtx.lock();
  orig_openat  = (orig_openat_f_type)dlsym(RTLD_NEXT,"openat");
  
  if (flags & O_CREAT)
    { 
      va_list arg; 
      va_start(arg, flags); 
      mode = va_arg(arg, int); 
      va_end(arg); 
      retcode=orig_openat(dirfd,pathname,flags,mode); 
    } 
  else
    {
      retcode=orig_openat(dirfd,pathname,flags);      
    }

  if (retcode>=0)
    {
      fd=retcode;
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
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }

  std::string str;
  std::ostringstream oss;
  oss << "openat("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
  log.add(oss.str());
  //mtx.unlock();
  return retcode;
}

extern int openat64(int dirfd, const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;
  orig_openat64_f_type orig_openat64;

  //mtx.lock();
  orig_openat64  = (orig_openat64_f_type)dlsym(RTLD_NEXT,"openat64");
  
  if (flags & O_CREAT)
    { 
      va_list arg; 
      va_start(arg, flags); 
      mode = va_arg(arg, int); 
      va_end(arg); 
      retcode=orig_openat64(dirfd,pathname,flags,mode); 
    } 
  else
    {
      retcode=orig_openat64(dirfd,pathname,flags);      
    }

  if (retcode>=0)
    {
      fd=retcode;
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
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }

  std::string str;
  std::ostringstream oss;
  oss << "openat64("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
  log.add(oss.str());
  //mtx.unlock();
  return retcode;
}

extern int    open(const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;
  orig_open_f_type orig_open;

  //mtx.lock();
  orig_open  = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  
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
  
  if (retcode>=0 && myiio.getStatus()==IIO_RUNNING)
    {
      fd=retcode;
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
	}
    }

  std::string str;
  std::ostringstream oss;
  oss << "open("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
  log.add(oss.str());
  //mtx.unlock();
  return retcode;
}

extern int open64(const char *pathname, int flags,...)
{
  int retcode;
  int mode=0;
  int fd;

  //mtx.lock();
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
    if (retcode>=0)
    {
      fd=retcode;
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
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }

  std::string str;
  std::ostringstream oss;
  oss << "open64("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
  log.add(oss.str());
  //mtx.unlock();
  return retcode;
}

int close(int fd)
{
  int retcode;

  orig_close_f_type orig_close;

  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;

  std::string str;
  std::ostringstream oss;

  //mtx.lock();
  oss << "close("<<fd<<")\n"; 
  log.add(oss.str());
  
  orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");

  gettimeofday(&tv0,&tz);
  retcode=orig_close(fd);
  gettimeofday(&tv1,&tz);

  if (myiio.getStatus()!=IIO_RUNNING) { return retcode; }
  
  if (retcode==0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setOldFd(fd);
	  ifi.setFd(-1);
	  //ifi.setName(getStrFDInfo(fd));
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
  //mtx.unlock();
  return retcode;
}

int fclose(FILE * FD)
{
  int retcode;
  int fd;

  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  
  orig_fclose_f_type orig_fclose;

  //mtx.lock();
  fd=fileno(FD);
  orig_fclose = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");

  std::string str;
  std::ostringstream oss;
  oss << "fclose("<<fd<<")\n"; 
  log.add(oss.str());


  gettimeofday(&tv0,&tz);
  retcode=orig_fclose(FD);
  gettimeofday(&tv1,&tz);

  if (retcode==0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setOldFd(fd);
	  ifi.setFd(-1);
	  //ifi.setName(std::string("UNKNOWN-FCLOSE"));
	  //ifi.setName(getStrFDInfo(fd));
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
  //mtx.unlock();
  return retcode;
}



int dup(int oldfd)
{
  int retcode;
  int newfd;
  
  orig_dup_f_type orig_dup;
  //mtx.lock();
  orig_dup = (orig_dup_f_type)dlsym(RTLD_NEXT,"dup");
  retcode=orig_dup(oldfd);
  newfd=retcode;

  if (retcode>=0)
    {
      if (myiio.existFd(oldfd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(newfd);
	  ifi.setOldFd(oldfd);
	  ifi.setName(std::string("UNKNOWN-DUP"));
	  ifi.setState(IOBYFILE_DUP);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(oldfd).setFd(newfd);
	  myiio.getFd(newfd).setState(IOBYFILE_DUP);
	  myiio.getFd(newfd).setOldFd(oldfd);
	}
    }
  //mtx.unlock();
  return retcode;
}




extern int dup2(int oldfd, int newfd)
{
  int retcode;
  std::string str;
  orig_dup2_f_type orig_dup2;
  //mtx.lock();
  orig_dup2 = (orig_dup2_f_type)dlsym(RTLD_NEXT,"dup2");
  retcode=orig_dup2(oldfd,newfd);

  if (retcode>=0)
    {
      if (myiio.existFd(oldfd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(newfd);
	  ifi.setOldFd(oldfd);
	  //ifi.setName(std::string("UNKNOWN-DUP2"));
	  ifi.setName(getStrFDInfo(newfd));
	  ifi.setState(IOBYFILE_DUP2);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(oldfd).setFd(newfd);
	  myiio.getFd(newfd).setState(IOBYFILE_DUP2);
	  myiio.getFd(newfd).setOldFd(oldfd);
	  //str=myiio.getFd(newfd).dump();
	}
    }


  std::ostringstream oss;
  oss << "dup2("<< oldfd << "," << newfd << ")=" << retcode << "\n";
  //oss << "dup2("<< oldfd << "," << newfd << ")=" << retcode << str << "\n";
  log.add(oss.str());

  //mtx.unlock();
  return retcode;
}


int dup3(int oldfd, int newfd, int flags)
{
  int retcode;

  orig_dup3_f_type orig_dup3;
  //mtx.lock();
  orig_dup3 = (orig_dup3_f_type)dlsym(RTLD_NEXT,"dup3");
  retcode=orig_dup3(oldfd,newfd,flags);

  if (retcode>=0)
    {
      if (myiio.existFd(oldfd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(newfd);
	  ifi.setOldFd(oldfd);
	  ifi.setName(std::string("UNKNOWN-DUP3"));
	  ifi.setState(IOBYFILE_DUP3);
	  myiio.iiof.push_back(ifi);
	}
      else
	{
	  myiio.getFd(oldfd).setFd(newfd);
	  myiio.getFd(newfd).setState(IOBYFILE_DUP3);
	  myiio.getFd(newfd).setOldFd(oldfd);
	}
    }
  //mtx.unlock();
  return retcode;
}







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
