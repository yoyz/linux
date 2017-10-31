/*
  AUTHOR           : johann peyrard peyrard.johann@gmail.com 2016/11/29
  NOTE             : use a rhel6/7 gcc 4.4 which works fine 
  TO BUILD         : $ g++ -fPIC -std=c++0x -ldl -shared inspectio7.cpp -o inspectio.so
  TO TEST          : $ $ mkdir -p $PWD/LOG ; rm -f $PWD/LOG/* ; export INSPECTIO_DUMP=$PWD/LOG ; unset INSPECTIO_ALL ; export LD_PRELOAD=./inspectio.so ; dd if=/dev/zero of=/dev/null bs=100M count=50 ; cat $PWD/LOG/*
  DEBUGON          : $ export INSPECTIO_ALL=vbla
  PATH TO OUTPUT   : $ export INSPECTIO_DUMP=/home_nfs/peyrardj/monapps/log
 */

/*
  DESIGN:
   - Every meaningfull libc call should be trapped
   - when the lib is loaded a IIo is created and a log file is created in $INSPECTIO_DUMP directory, if this directory is not available, it will crash...
   - class Logger,  log every call with input parameter and return value, 
     it is print only if INSPECTIO_ALL=something

   - class Iaccess, should be aware of all access in a Ifile, 4k, 8k, seek...
   - class Ifile,   track the file name, filedescriptor from born to death 
                    contain an Iaccess object
   - class IIo,     contain a list of Ifile
  
 */
#define M1 1000000
#define STREAMLOG() do {						\
    int rank=-1;							\
    char rank_str[16];                                                   \
    if (getenv_OMPI_COMM_WORLD_RANK != NULL)  rank=atoi(getenv_OMPI_COMM_WORLD_RANK); \
    if (getenv_PMI_RANK             != NULL)  rank=atoi(getenv_PMI_RANK); \
    if (getenv_PMIX_RANK            != NULL)  rank=atoi(getenv_PMIX_RANK); \
    sprintf(rank_str,"%08d",rank);					\
    if (getenv_INSPECTIO_DUMP!=NULL)					\
      {									\
	if (rank==-1)							\
	  stream_logfile   << getenv_INSPECTIO_DUMP << "/" << "inspectiolog." << hostname << "." << "nompi"  << "." << mypid; \
	else								\
	  stream_logfile   << getenv_INSPECTIO_DUMP << "/" << "inspectiolog." << hostname << "." << rank_str << "." << mypid; \
	if (rank==-1)							\
	  stream_bwgnuplot << getenv_INSPECTIO_DUMP << "/" << "bwgplot."      << hostname << "." << "nompi"  << "." << mypid; \
	else								\
	  stream_bwgnuplot << getenv_INSPECTIO_DUMP << "/" << "bwgplot."      << hostname << "." << rank_str << "." << mypid; \
      }									\
    else								\
      {									\
	if (getenv_HOME!=NULL)						\
	  stream_logfile   << getenv_HOME           << "/" << "inspectiolog."<< hostname << "." << mypid; \
	if (getenv_HOME!=NULL)						\
	  stream_bwgnuplot << getenv_HOME           << "/" << "bwgplot."     << hostname << "." << mypid; \
      }									\
  } while(0) 


#include <string>
#include <string.h>    // memset
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>  //getpid
#include <sys/stat.h>
#include <sys/time.h>   // gettimeofday
#include <fcntl.h>      // openat pipe pipe2
#include <stdarg.h>     // va_arg
#include <mutex>        // std::mutex

#include <unistd.h>
#include <dirent.h>     // opendir/closedir
#include <execinfo.h>   // backtrace
#include <signal.h>     // signal



#define MAX_FILEDESC 1024

char * getenv_HOME;
char * getenv_OMPI_COMM_WORLD_RANK;
char * getenv_PMI_RANK;
char * getenv_PMIX_RANK;
char * getenv_INSPECTIO_DUMP;
char * getenv_INSPECTIO_GPLOT;
char * getenv_INSPECTIO_ALL;
int    mypid;
std::mutex mtx_iio;
std::mutex mtx_iio2;
std::mutex mtx;
std::mutex mtx_logger;
std::mutex mtx_bwgplot;  
std::string str_log;

std::string getStrFDInfo( long fd );

typedef void     (*orig_init_f_type)();
typedef void     (*orig_fini_f_type)();

typedef int      (*orig_closedir_f_type)(DIR *dirp);


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
typedef int     (*orig_creat_f_type)(const char *pathname, mode_t mode);


typedef int     (*orig_fprintf_f_type)(FILE *stream, const char *format, ...);

//size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
//typedef ssize_t (*orig_fread_f_type)(void *ptr, size_t size, size_t nbemb, FILE *stream);
//typedef FILE * (*orig_fopen_f_type)(const char *path, const char *mode);



enum ioByFileState
  {
    IOBYFILE_UNKNOW_ASSUME_CLOSED,    // 0
    IOBYFILE_NEW,                     // 1
    IOBYFILE_OPEN,                    // 2
    IOBYFILE_READ,                    // 3
    IOBYFILE_WRITE,                   // 4
    IOBYFILE_CLOSE,                   // 5
    IOBYFILE_SEEK,                    // 6
    IOBYFILE_DUP,                     // 7
    IOBYFILE_DUP2,                    // 8
    IOBYFILE_DUP3,                    // 9
    IOBYFILE_CREAT,                   // 10
  };

enum iioStatus
  {
    IIO_BEGIN,                        // 0
    IIO_RUNNING,                      // 1
    IIO_ENDING,                       // 2
  };


void backtrace_handler()
{
  void *array[30];
  size_t size;
  
  // get void*'s for all entries on the stack
  size = backtrace(array, 30);
  
  // print out all the frames to stderr
  fprintf(stderr, "Error\n");
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void sighandler(int signum)
{
  backtrace_handler();
  //printf("Caught signal %d, coming out...\n", signum);
  //exit(1);
}

//################################################################################
class BWGnuplot
{
public:
  BWGnuplot();  
  void add(struct timeval tv0, struct timeval tv1,int writesize, int readsize);
  void flush();
  char * genv;
  int64_t last_step;
  int64_t current_step;
  int64_t first_step;
  int64_t write;
  int64_t read;
  int64_t write_time;
  int64_t read_time;
  std::vector<std::string> gplotstr;
};

BWGnuplot::BWGnuplot()
{
  first_step=0;
  last_step=0; 
  current_step=0;
  write_time=0;
  read_time=0;
  write=0;
  read=0;
}



void BWGnuplot::add(struct timeval tv0, struct timeval tv1, int writesize, int readsize)
{
  std::ostringstream oss;
  int64_t timestart=tv0.tv_sec*M1+tv0.tv_usec;
  int64_t duration=(tv1.tv_sec-tv0.tv_sec)*M1 + tv1.tv_usec-tv0.tv_usec;
  int64_t timestop=timestart+duration;
  int64_t bandwidthwrite;
  int64_t bandwidthread;
  float   time_f;
  int64_t time_d;
  int64_t i,j;
  char    str[128];
  //const char * format="%8f\t%12llu\t%12llu\t%8llu\t%8llu\n";
  const char * format="%8llu\t%12llu\t%12llu\t%8llu\t%8llu\n";

  mtx_bwgplot.lock();

  if (last_step==0)   //INIT step we get the "time" 
    {
      first_step=timestart;
      last_step=timestop;
      current_step=timestop;
    }

  // increment the counters time and write/read
  if (writesize) { write_time = write_time + duration; write=write+writesize;   }
  if (readsize)  { read_time  = read_time  + duration;  read=read+readsize;     }

  if (timestop-last_step>M1)
    {
      // There is more than 2s without IO ( 2*M1 ),
      // so we calculate the bandwidth for one second
      // and we increment the last_step by one second,
      if ((timestop-last_step)>(2*M1))
	{
	  bandwidthwrite=(write)/(M1);
	  bandwidthread=(read)/(M1);
	  time_f=(last_step-first_step);
	  time_f=(time_f/ M1)+1;
	  time_d=time_f;
	  oss.str("");
	  snprintf(str,128,
		   format,
		   time_d,//(last_step-first_step) / M1,
		   write,
		   read,
		   bandwidthwrite,
		   bandwidthread);
	  oss << str;
	  gplotstr.push_back(oss.str());
	  last_step=last_step+M1;     // here we jump one second 
	  write=0;
	  read=0;
	  write_time=0;
	  read_time=0;
	}
      
      // we fall here
      // there is no IO we have marked it,
      // but the last_step is not below one second
      // so we send some 0 to have one line by second
      if (( (timestop-last_step) > M1 ) &&
	  ((write==0) && (read==0)))
	{
	  j=0;
	  for (i=last_step; i<timestop;i=i+M1)
	    {
	      time_f=i-first_step;
	      time_f=(time_f/M1)+1;
	      time_d=time_f;
	      oss.str("");
	      snprintf(str,128,
		       format,
		       time_d,//(i-first_step) / M1,
		       0,
		       0,
		       0,
		       0);
	      oss << str;
	  
	      gplotstr.push_back(oss.str());
	      j++;
	    }
	  //last_step=timestart+duration;
	  last_step=last_step+j*M1;
	}	  	  

      // There is more than 1s since the last line
      // so we calculate the bandwidth for one second
      // and we increment the last_step by one second,
      if (( (timestop-last_step) > M1) &&
	  ((write>0) || (read>0)))	      
	{
	  bandwidthwrite=(write)/(timestop-last_step);
	  bandwidthread=(read)/(timestop-last_step);
	  time_f=timestop-first_step;
	  time_f=time_f/ M1;
	  time_d=time_f;
	  oss.str("");
	  snprintf(str,128,
		   format,
		   time_d,//(timestart-first_step) / M1,
		   write,
		   read,
		   bandwidthwrite,
		   bandwidthread);
	  oss << str;
	  gplotstr.push_back(oss.str());
	  last_step=last_step+M1;
	  write=0;
	  read=0;
	  write_time=0;
	  read_time=0;
	}
    }
  //last_step=timestart+duration;
  mtx_bwgplot.unlock();
}




BWGnuplot bwgplot;

//################################################################################


class Logger
{
public:
  Logger();
  void add(std::string str);
  std::vector<std::string> logstr;
  char * genv;
};

 
Logger::Logger() 
{
  //genv=getenv("INSPECTIO_ALL");
  //getenv_INSPECTIO_ALL=getenv("INSPECTIO_ALL");  
}

void Logger::add(std::string str)
{
  if (getenv_INSPECTIO_ALL)
    {
      mtx_logger.lock();
      logstr.push_back(str);
      mtx_logger.unlock();
    }
}

Logger  inspectio_log;

//################################################################################

class Iaccess
{
public:
  Iaccess();
  ~Iaccess();
  unsigned long long   readcall;
  unsigned long long   writecall;

  //unsigned long long   readtime_seconds;
  //unsigned long long   writetime_seconds;

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

   //readtime_seconds=0;
   //writetime_seconds=0;

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

//################################################################################

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
  std::string dumpHeader();
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


std::string Ifile::dumpHeader()
{
  char str[2048];

  //sprintf(str,"SUMAR[    CALL    MBYTE      SEC           CALL    MBYTE      SEC      FD OLDFD STATE NAME\n");
  sprintf(str,"SUMARY[    CALL   MBYTE      SEC           CALL   MBYTE     SEC       FD OLDFD STATE NAME\n");
  return std::string(str);
}

std::string Ifile::dump()
{
  char str[2048];
  char * genv;
  double f_writesize_MB;
  double f_readsize_MB;
  double f_writetime_seconds;
  double f_readtime_seconds;
  
  f_writesize_MB=((double)(iac.writesize))/1000.0/1000.0;
  f_writetime_seconds=((double)(iac.writetime_useconds))/1000.0/1000.0;
  f_readsize_MB=((double)(iac.readsize))/1000.0/1000.0;
  f_readtime_seconds=((double)(iac.readtime_useconds))/1000.0/1000.0;
  
  //genv=getenv("INSPECTIO_ALL");
  //sprintf(str,"WRITE[%8lld %8lld %8lld] READ[%8lld %8lld %8lld] [%5d %5d %5d %s] \n",
  sprintf(str,"WRITE [%8lld %7.1f %7.1f ] READ[%8lld %7.1f %7.1f ]  [%5d %5d %5d %s] \n",
	  iac.writecall,
	  f_writesize_MB,
	  f_writetime_seconds,
	  iac.readcall,
	  f_readsize_MB,
	  f_readtime_seconds,
	  fd,
	  oldfd,
	  state,
	  name.c_str()
	  );
  // if we don't want all output : $ unset INSPECTIO_ALL
  // We don't display write and read under 1MB of working set, to reduce line noize
  if (getenv_INSPECTIO_ALL==NULL && iac.writesize/1000/1000 == 0 && iac.readsize/1000/1000 == 0)
    sprintf(str,"");

  return std::string(str);
}

//################################################################################

class Iio
{
public:
  Iio();
  ~Iio();
  void noop();
  void dump();
  int existFd(int fdtoseek);
  Ifile & getFd(int fdtoseek);
  Ifile & getTrashFd(int fdtoseek);
  int moveTrash(int fdtoseek);
  int getStatus() { return status; }
  //void dump
  std::list<Ifile> iiof;
  std::list<Ifile> iiof_trash;
  
  int a;
  int status; // 0=begin 1=running 2=end
};


int Iio::moveTrash(int fdtoseek)
{
  mtx_iio2.lock();
  for (std::list<Ifile>::iterator iter = iiof.begin() ; iter != iiof.end(); iter++)
    {
      if (iter->fd==fdtoseek)
	{
	  Ifile ifi=*iter;	  
	  iiof.erase(iter);
	  iiof_trash.push_back(ifi);
	  mtx_iio2.unlock();
	  return 1;
	}
    }
  mtx_iio2.unlock();
  return 0;
}

Ifile & Iio::getFd(int fdtoseek)
{
  mtx_iio2.lock();
  int i;
  for (std::list<Ifile>::iterator iter = iiof.begin() ; iter != iiof.end(); iter++)
    if (iter->fd==fdtoseek)
      {
	mtx_iio2.unlock();
	return *iter;
      }

  printf("ERROR Iio::getFd(%d) not found",fdtoseek);
  backtrace_handler();
  mtx_iio2.unlock();
  exit(1);
}


Ifile & Iio::getTrashFd(int fdtoseek)
{
  mtx_iio2.lock();
  int i;
  for (std::list<Ifile>::iterator iter = iiof_trash.begin() ; iter != iiof_trash.end(); iter++)
    if (iter->fd==fdtoseek)
      {
	mtx_iio2.unlock();
	return *iter;
      }  
  printf("ERROR Iio::getTrashFd(%d) not found",fdtoseek);
  backtrace_handler();
  mtx_iio2.unlock();
  exit(1);
}


int Iio::existFd(int fdtoseek)
{
  mtx_iio2.lock();
  int i=0;
  int ret=-1;
  for (std::list<Ifile>::iterator iter = iiof.begin() ; iter != iiof.end(); iter++)
    if (iter->fd==fdtoseek)
      {
	mtx_iio2.unlock();
	return 1;
      }
  mtx_iio2.unlock();
  return ret;
}


void Iio::noop()
{
  a=1;
}


Iio::Iio()
{
  mtx_iio.lock();
  FILE * FD;
  DIR   * procselffd;
  //int    pid=1;
  int    rank=-1;
  char   hostname[1024];
  
  std::ostringstream stream_logfile;
  std::ostringstream stream_bwgnuplot;

  struct dirent *myfile_in_procselfd;
  struct stat mystat;
  std::string strStarDot(".");
  std::string strStarDotdot("..");

  // grab the environement variable one time
  getenv_OMPI_COMM_WORLD_RANK = getenv("OMPI_COMM_WORLD_RANK");
  getenv_PMI_RANK             = getenv("PMI_RANK");
  getenv_PMIX_RANK            = getenv("PMIX_RANK");
  getenv_INSPECTIO_DUMP       = getenv("INSPECTIO_DUMP");
  getenv_HOME                 = getenv("HOME");
  getenv_INSPECTIO_GPLOT      = getenv("INSPECTIO_GPLOT");
  getenv_INSPECTIO_ALL        = getenv("INSPECTIO_ALL");
  mypid=getpid();
  //printf("loading\n");
  orig_fopen_f_type orig_fopen;
  orig_fclose_f_type orig_fclose;
  status=IIO_BEGIN;
  //mtx.lock();
  orig_fopen   = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");
  gethostname(hostname, 1024);


  STREAMLOG();

  
  FD=orig_fopen(stream_logfile.str().c_str(),"w");
  
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
	}                 
    }

  status=IIO_RUNNING;
  fclose(FD);
  closedir(procselffd);
  mtx_iio.unlock();
}

Iio::~Iio()
{
  mtx_iio.lock();
  FILE * FD;
  //pid_t pid=1;
  int   rank=-1;
  char logfile[1024];
  char   hostname[1024];
  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;
  std::ostringstream stream_logfile;
  std::ostringstream stream_bwgnuplot;
  gethostname(hostname, 1024);

  status=IIO_ENDING;

  orig_fopen   = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");
  
  //pid=getpid();
  
  STREAMLOG();
  
  dump();
  FD=orig_fopen(stream_logfile.str().c_str(),"a");
  orig_fclose(FD);
  mtx_iio.unlock();
}




void Iio::dump()
{
  FILE * FD;
  FILE * FDGPLOT;
  //pid_t pid=1;
  int   rank=-1;
  char logfile[1024];
  char   hostname[1024];
  //int pid=1;
  int i;
  int64_t readtime_useconds=0;
  int64_t writetime_useconds=0;
  int64_t read_size=0;
  int64_t write_size=0;
  double f_writesize_MB=0.0;
  double f_writetime_seconds=0.0;
  double f_readsize_MB=0.0;
  double f_readtime_seconds=0.0;

  char * genv;
  std::ostringstream stream_logfile;
  std::ostringstream stream_bwgnuplot;
  orig_fopen_f_type  orig_fopen;
  orig_fclose_f_type orig_fclose;


  
  gethostname(hostname, 1024);
  //genv=getenv("INSPECTIO_ALL");

  orig_fopen   = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  orig_fclose  = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");

  STREAMLOG();
  
  FD      = orig_fopen(stream_logfile.str().c_str(),"a");

  FDGPLOT = orig_fopen(stream_bwgnuplot.str().c_str(),"w");


  // Calculate IOTIME in usecond and total IOSize in byte

    for (std::list<Ifile>::iterator iter = iiof.begin() ; iter != iiof.end(); iter++)
      {
	     write_size+=iter->iac.writesize;      
	     read_size+=iter->iac.readsize;
	     
	     writetime_useconds+=iter->iac.writetime_useconds;  
	     readtime_useconds+=iter->iac.readtime_useconds;
      }
    for (std::list<Ifile>::iterator iter = iiof_trash.begin() ; iter != iiof_trash.end(); iter++)
      {
	     write_size+=iter->iac.writesize;      
	     read_size+=iter->iac.readsize;
	     
	     writetime_useconds+=iter->iac.writetime_useconds;  
	     readtime_useconds+=iter->iac.readtime_useconds;
      }
  
  // convert IOTIME in second and MByte
  f_writesize_MB=((double)write_size)/1000.0/1000.0;
  f_writetime_seconds=((double)(writetime_useconds))/1000.0/1000.0;
  f_readsize_MB=((double)read_size)/1000.0/1000.0;
  f_readtime_seconds=((double)(readtime_useconds))/1000.0/1000.0;

  // if there is IOTime, dump the header and write each line
  if (readtime_useconds+writetime_useconds)
    {
      fprintf(FD,iiof.begin()->dumpHeader().c_str());
  
      for (std::list<Ifile>::iterator iter = iiof.begin() ; iter != iiof.end(); iter++)
	{
	  fprintf(FD,iter->dump().c_str());
	}
      for (std::list<Ifile>::iterator iter = iiof_trash.begin() ; iter != iiof_trash.end(); iter++)
	{
	  fprintf(FD,iter->dump().c_str());
	}
      
      // if there is no catched time spent in IO, ok it's a lie, but should be easier to understand
      // print BW=0 
      if (readtime_useconds+writetime_useconds)
	fprintf(FD,"IOTIME[         %7.1f %7.1f ] R/W [         %7.1f %7.1f    ]BW[ %7.1f MB/s ]\n",
		f_writesize_MB,
		f_writetime_seconds,
		f_readsize_MB,
		f_readtime_seconds,
		(f_readsize_MB+f_writesize_MB)/(f_readtime_seconds+f_writetime_seconds)
		);
    }
  
  // dump the 'strace -e file'
  if (getenv_INSPECTIO_ALL!=NULL)
    for (i=0;i<inspectio_log.logstr.size();i++)
      fprintf(FD,inspectio_log.logstr[i].c_str());
  orig_fclose(FD);
  
  for (i=0;i<bwgplot.gplotstr.size();i++)
    fprintf(FDGPLOT,bwgplot.gplotstr[i].c_str());
  orig_fclose(FDGPLOT);


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
      mtx_iio2.lock();
      myiio.iiof.push_back(ifi);
      mtx_iio2.unlock();
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
      mtx_iio2.lock();
      myiio.iiof.push_back(ifi);
      mtx_iio2.unlock();
    }
  myiio.getFd(fd).iac.readcall++;
  myiio.getFd(fd).iac.readsize+=count;
}


void add_write_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
  ////mtx.lock();
  if (myiio.existFd(fd)==-1)
    {
      Ifile ifi;
      ifi.setFd(fd);
      mtx_iio2.lock();
      myiio.iiof.push_back(ifi);
      mtx_iio2.unlock();
    }
  myiio.getFd(fd).iac.writetime_useconds+=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
  ////mtx.unlock();
}

void add_read_time(int fd,  struct timeval tv0,  struct timeval tv1)
{
  // //mtx.lock();
  if (myiio.existFd(fd)==-1)
    {
      Ifile ifi;
      ifi.setFd(fd);
      mtx_iio2.lock();
      myiio.iiof.push_back(ifi);
      mtx_iio2.unlock();
    }
  myiio.getFd(fd).iac.readtime_useconds+=(tv1.tv_sec-tv0.tv_sec)*1000000 + tv1.tv_usec-tv0.tv_usec;
  ////mtx.unlock();
}


int fprintf(FILE *stream, const char *format, ...)
{
  int retcode;
  int fd;
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz; 
  std::ostringstream oss;
  orig_fprintf_f_type orig_fprintf;
  orig_fprintf  = (orig_fprintf_f_type)dlsym(RTLD_NEXT,"fprintf");

  va_list args;
  va_start(args, format);
  va_end(args);

  gettimeofday(&tv0,&tz);
  retcode=vfprintf(stream, format, args);  
  gettimeofday(&tv1,&tz);

  fd=fileno(stream);
  if (myiio.getStatus()!=IIO_RUNNING) { return retcode; }
  
  mtx.lock();
  if (retcode>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN-FPRINTF"));
	  ifi.setState(IOBYFILE_WRITE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_write_count(fd,retcode);
      add_write_time(fd,tv0,tv1);
    }
  if (getenv_INSPECTIO_ALL)
    {
      oss << "fprintf(" << stream << "(" << fd << ")" << "," << format << ")="<< retcode << "\n";
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,retcode,0);
  mtx.unlock();
  
  return retcode;
}



extern "C" {
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
  std::string str;
  std::ostringstream oss;

  ////mtx.lock();
  

  fd=fileno(stream);


  orig_fwrite  = (orig_fwrite_f_type)dlsym(RTLD_NEXT,"fwrite");

  gettimeofday(&tv0,&tz);
  retsize=orig_fwrite(ptr,size,nmemb,stream);
  gettimeofday(&tv1,&tz);

  if (myiio.getStatus()!=IIO_RUNNING) { return retsize; }
  
  mtx.lock();
  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN-FWRITE"));
	  //ifi.setName(getStrFDInfo(fd));
	  ifi.setState(IOBYFILE_WRITE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_write_count(fd,size*nmemb);
      add_write_time(fd,tv0,tv1);
    }
  if (getenv_INSPECTIO_ALL)
    {
      oss << "fwrite(" << ptr << "(" << fd << ")" << "," << size << "," << nmemb << "," << stream << ")="<< retsize << "\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,size*nmemb,0);
  mtx.unlock();
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
  std::string str;
  std::ostringstream oss;

  ////mtx.lock();
  

  fd=fileno(stream);


  orig_fread  = (orig_fread_f_type)dlsym(RTLD_NEXT,"fread");

  gettimeofday(&tv0,&tz);
  retsize=orig_fread(ptr,size,nmemb,stream);
  gettimeofday(&tv1,&tz);


  mtx.lock();
  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN-FWRITE"));
	  //ifi.setName(getStrFDInfo(fd));
	  ifi.setState(IOBYFILE_WRITE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_read_count(fd,retsize);
      add_read_time(fd,tv0,tv1);
    }
  if (getenv_INSPECTIO_ALL)
    {
      oss << "fread(" << ptr << "(" << fd << ")" << "," << size << "," << nmemb << "," << stream << ")="<< retsize << "\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,0,size*nmemb);
  mtx.unlock();
  return retsize;
}



size_t fwrite64(const void *ptr, size_t size, size_t nmemb,FILE *stream)
{  
  return fwrite(ptr,size,nmemb,stream);
}

ssize_t write(int fd, const void *buf, size_t count)
{
  int size;
  int readsize=0;
  int size_t_count=count;
  orig_write_f_type orig_write;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  std::string str;
  std::ostringstream oss;

  ////mtx.lock(); 
  orig_write  = (orig_write_f_type)dlsym(RTLD_NEXT,"write");

  gettimeofday(&tv0,&tz);
  size=orig_write(fd,buf,count);
  gettimeofday(&tv1,&tz);


  mtx.lock();
  if (size>=0)
    {
      ////mtx.lock();
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  //ifi.setName(getStrFDInfo(fd));
	  ifi.setName(std::string("UNKNOWN-WRITE"));
	  ifi.setState(IOBYFILE_WRITE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
      ////mtx.unlock();
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "write(" << fd << "," << buf << "," << count << ")=" << size << "\n";
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,size,0);
  mtx.unlock();
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
  std::string str;
  std::ostringstream oss;


  ////mtx.lock(); 
  orig_read  = (orig_read_f_type)dlsym(RTLD_NEXT,"read");

  gettimeofday(&tv0,&tz);
  size=orig_read(fd,buf,count);
  gettimeofday(&tv1,&tz);


  if (myiio.getStatus()!=IIO_RUNNING) { return size; }
  mtx.lock();
  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  //ifi.setName(getStrFDInfo(fd));
	  ifi.setName(std::string("UNKNOWN-READ"));
	  ifi.setState(IOBYFILE_READ);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_read_count(fd,size);
      add_read_time(fd,tv0,tv1);
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "read(" << fd << "," << buf << "," << count << ")=" << size << "\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,0,size);
  mtx.unlock();
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
  std::string str;
  std::ostringstream oss;

  ////mtx.lock();
  orig_pwrite  = (orig_pwrite_f_type)dlsym(RTLD_NEXT,"pwrite");

  gettimeofday(&tv0,&tz);
  size=orig_pwrite(fd,buf,count,offset);
  gettimeofday(&tv1,&tz);


  mtx.lock();
  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN-PWRITE"));
	  ifi.setState(IOBYFILE_WRITE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "pwrite(" << fd << "," << buf << "," << count << ")\n";
      inspectio_log.add(oss.str());
    }

  bwgplot.add(tv0,tv1,size,0);
  mtx.unlock();
  return size;
}

ssize_t pwrite64(int fd, const void *buf, size_t count,off_t offset)
{
  pwrite(fd,buf,count,offset);
}



ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
  ssize_t size=0;
  int retsize; 
  orig_writev_f_type orig_writev;
  //int gettimeofday(struct timeval *tv, struct timezone *tz);
  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  std::string str;
  std::ostringstream oss;
  int i;

  for (i=0;i<iovcnt;i++)
	size+=iov[i].iov_len;
  ////mtx.lock();
  orig_writev  = (orig_writev_f_type)dlsym(RTLD_NEXT,"writev");

  gettimeofday(&tv0,&tz);
  size=orig_writev(fd,iov,iovcnt);
  gettimeofday(&tv1,&tz);

  mtx.lock();
  if (size>=0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string("UNKNOWN-WRITEV"));
	  //ifi.setName(getStrFDInfo(fd));
	  ifi.setState(IOBYFILE_WRITE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      add_write_count(fd,size);
      add_write_time(fd,tv0,tv1);
    }


  if (myiio.getStatus()!=IIO_RUNNING) { return retsize; }


  if (getenv_INSPECTIO_ALL)
    {      
      oss << "writev(" << fd << "," << iovcnt << ")\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,size,0);
  mtx.unlock();
  
  return size;
}

ssize_t writev64(int fd, const struct iovec *iov, int iovcnt)
{
  //printf("*******************\n");
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
  std::string str;
  std::ostringstream oss;

  ////mtx.lock();
  orig_pwritev  = (orig_pwritev_f_type)dlsym(RTLD_NEXT,"pwritev");
  size=orig_pwritev(fd,iov,iovcnt,offset);

  mtx.lock();
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "pwritev(" << fd << "," << iovcnt << ")\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,size,0);
  mtx.unlock();
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
  std::string str;
  std::ostringstream oss;

  
  
  orig_fopen  = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
  FP=orig_fopen(pathname,mode);
  
  if (myiio.getStatus()!=IIO_RUNNING) { return FP; }
  if (FP!=0)
    fd=fileno(FP);
  
  mtx.lock();  
  if (FP!=0)
  {

      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}      
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	  myiio.getFd(fd).setFd(fd);
	  //myiio.getFd(fd).setOldFd(fd);
	}
      if (getenv_INSPECTIO_ALL)
	{      
	  oss << "fopen("<<pathname<< mode << ")="<< FP<< "(" << fd << ")" << "\n"; 
	  inspectio_log.add(oss.str());
	}
  }
  
  mtx.unlock();
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
  std::string str;
  std::ostringstream oss;

  
  orig_fdopen  = (orig_fdopen_f_type)dlsym(RTLD_NEXT,"fdopen");
   
  file=orig_fdopen(fd,mode);
  
  mtx.lock();
  if (file!=NULL)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	}
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "fdopen("<<fd<< ","<< mode << ")=" << file << "\n"; 
      inspectio_log.add(oss.str());
    }
  mtx.unlock();
  return file;  
}

FILE *fdopen64(int fd, const char *mode)
{
  return fdopen(fd,mode);
}



  //extern int openat(int dirfd, const char *pathname, int flags,mode_t mode )
  extern int openat(int dirfd, const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;
  orig_openat_f_type orig_openat;
  std::string str;
  std::ostringstream oss;


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
  mtx.lock();
  if (retcode>=0)
    {
      fd=retcode;
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "openat("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
      inspectio_log.add(oss.str());
    }
  mtx.unlock();
  return retcode;
}




  
extern int openat64(int dirfd, const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;
  orig_openat64_f_type orig_openat64;
  std::string str;
  std::ostringstream oss;

  
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
  mtx.lock();
  if (retcode>=0)
    {
      fd=retcode;
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }
  if (getenv_INSPECTIO_ALL)
    {        
      oss << "openat64("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
      inspectio_log.add(oss.str());
    }
  mtx.unlock();
  return retcode;
}

extern int    open(const char *pathname, int flags,...)
{
  int retcode;
  int mode = 0;
  int fd;
  orig_open_f_type orig_open;
  std::string str;
  std::ostringstream oss;

  
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

  if (myiio.getStatus()!=IIO_RUNNING) { return retcode; } 
  mtx.lock();
  if (retcode>=0)
    {
      fd=retcode;
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	}
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "open("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
      inspectio_log.add(oss.str());
    }
  mtx.unlock();
  return retcode;
}

extern int open64(const char *pathname, int flags,...)
{
  int retcode;
  int mode=0;
  int fd;
  std::string str;
  std::ostringstream oss;

  
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
  
  mtx.lock();
  if (retcode>=0)
    {
      fd=retcode;
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_OPEN);
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "open64("<<pathname<< ","<< flags << ")=" << retcode << "\n"; 
      inspectio_log.add(oss.str());
    }
  mtx.unlock();
  return retcode;
}


int creat(const char *pathname, mode_t mode)
{
  int retcode;
  int fd;
  std::string str;
  std::ostringstream oss;
  orig_creat_f_type orig_creat;
  
  orig_creat  = (orig_creat_f_type)dlsym(RTLD_NEXT,"creat");  
  retcode=orig_creat(pathname,mode); 
  
  mtx.lock();
  if (retcode>=0)
    {
      fd=retcode;
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(fd);
	  ifi.setName(std::string(pathname));
	  ifi.setState(IOBYFILE_OPEN);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(fd).setState(IOBYFILE_CREAT);
	  myiio.getFd(fd).setName(std::string(pathname));
	}
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "creat("<<pathname<< ","<< mode << ")=" << retcode << "\n"; 
      inspectio_log.add(oss.str());
    }
  mtx.unlock();
  return retcode;
}

  
int closedir(DIR *dirp)
{
    int retcode;
    int fd;

    orig_closedir_f_type orig_closedir;
      
    orig_closedir = (orig_closedir_f_type)dlsym(RTLD_NEXT,"closedir");
    if (dirp!=NULL)
      fd=dirfd(dirp);
    retcode=orig_closedir(dirp);
    mtx.lock();
    if (myiio.existFd(fd)==-1)
      {
	Ifile ifi;
	ifi.setFd(-1);
	ifi.setState(IOBYFILE_CLOSE);
	mtx_iio2.lock();
	myiio.iiof.push_back(ifi);
	mtx_iio2.unlock();
      }
    else
      {
	myiio.getFd(fd).setState(IOBYFILE_CLOSE);
	myiio.getFd(fd).setOldFd(fd);
	myiio.getFd(fd).setFd(-1);
	//myiio.getFd(fd).setOldFd(fd);
      }    
    mtx.unlock();
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

  
  
  orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");

  gettimeofday(&tv0,&tz);
  retcode=orig_close(fd);
  gettimeofday(&tv1,&tz);

  if (myiio.getStatus()!=IIO_RUNNING) { return retcode; }

  mtx.lock();
  if (retcode==0)
    {
      if (myiio.existFd(fd)==-1)
	{
	  Ifile ifi;
	  ifi.setOldFd(fd);
	  ifi.setFd(-1);
	  ifi.setState(IOBYFILE_CLOSE);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  /*
	   add_write_time(fd,tv0,tv1);
	   myiio.getFd(fd).setState(IOBYFILE_CLOSE);
	   myiio.getFd(fd).setOldFd(fd);
	   myiio.getFd(fd).setFd(-1);
	  */
	   add_write_time(fd,tv0,tv1);
	   myiio.moveTrash(fd);
	   myiio.getTrashFd(fd).setState(IOBYFILE_CLOSE);
	   myiio.getTrashFd(fd).setOldFd(fd);
	   myiio.getTrashFd(fd).setFd(-1);

	}
    }
  //myiio.dump();
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "close("<<fd<<")\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,0,0);
  mtx.unlock();
  return retcode;
}

int fclose(FILE * FD)
{
  int retcode;
  int fd;

  struct timeval tv0;
  struct timeval tv1;
  struct timezone tz;
  std::string str;
  std::ostringstream oss;
  
  orig_fclose_f_type orig_fclose;

  
  fd=fileno(FD);
  orig_fclose = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");



  gettimeofday(&tv0,&tz);
  retcode=orig_fclose(FD);
  gettimeofday(&tv1,&tz);
  
  if (myiio.getStatus()!=IIO_RUNNING) { return retcode; }
  mtx.lock();
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
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  /*
	  myiio.getFd(fd).setState(IOBYFILE_CLOSE);
	  myiio.getFd(fd).setOldFd(fd);
	  myiio.getFd(fd).setFd(-1);
	  */
	  add_write_time(fd,tv0,tv1);
	   myiio.moveTrash(fd);
	   myiio.getTrashFd(fd).setState(IOBYFILE_CLOSE);
	   myiio.getTrashFd(fd).setOldFd(fd);
	   myiio.getTrashFd(fd).setFd(-1);

	}
    }
  if (getenv_INSPECTIO_ALL)
    {      
      oss << "fclose("<<fd<<")\n"; 
      inspectio_log.add(oss.str());
    }
  bwgplot.add(tv0,tv1,0,0);
  mtx.unlock();
  return retcode;
}



int dup(int oldfd)
{
  int retcode;
  int newfd;
  
  orig_dup_f_type orig_dup;
  
  orig_dup = (orig_dup_f_type)dlsym(RTLD_NEXT,"dup");
  retcode=orig_dup(oldfd);
  newfd=retcode;
  mtx.lock();
  if (retcode>=0)
    {
      if (myiio.existFd(oldfd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(newfd);
	  ifi.setOldFd(oldfd);
	  ifi.setName(std::string("UNKNOWN-DUP"));
	  ifi.setState(IOBYFILE_DUP);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(oldfd).setFd(newfd);
	  myiio.getFd(newfd).setState(IOBYFILE_DUP);
	  myiio.getFd(newfd).setOldFd(oldfd);
	}
    }
  mtx.unlock();
  return retcode;
}




extern int dup2(int oldfd, int newfd)
{
  int retcode;
  std::string str;
  orig_dup2_f_type orig_dup2;
  std::ostringstream oss;

  orig_dup2 = (orig_dup2_f_type)dlsym(RTLD_NEXT,"dup2");
  retcode=orig_dup2(oldfd,newfd);

  mtx.lock();
  if (retcode>=0)
    {
      // close virtually the newFD because dup(2) manual
      if (myiio.existFd(newfd)!=-1)
	{
	  //printf("THERE %d ",myiio.existFd(newfd));
	  myiio.getFd(newfd).setOldFd(newfd);
	  myiio.getFd(newfd).setFd(-1);
	}
      
      if (myiio.existFd(oldfd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(newfd);
	  ifi.setOldFd(oldfd);
	  ifi.setName(std::string("UNKNOWN-DUP2"));
	  //ifi.setName(getStrFDInfo(newfd));
	  ifi.setState(IOBYFILE_DUP2);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  //printf("HERE");
	  myiio.getFd(oldfd).setFd(newfd);
	  myiio.getFd(newfd).setState(IOBYFILE_DUP2);
	  myiio.getFd(newfd).setOldFd(oldfd);
	  //str=myiio.getFd(newfd).dump();
	}
    }

  if (getenv_INSPECTIO_ALL)
    {      
      oss << "dup2("<< oldfd << "," << newfd << ")=" << retcode << "\n";
      inspectio_log.add(oss.str());
    }

  mtx.unlock();
  return retcode;
}


int dup3(int oldfd, int newfd, int flags)
{
  int retcode;

  orig_dup3_f_type orig_dup3;
  
  orig_dup3 = (orig_dup3_f_type)dlsym(RTLD_NEXT,"dup3");
  retcode=orig_dup3(oldfd,newfd,flags);

  mtx.lock();
  if (retcode>=0)
    {
      if (myiio.existFd(oldfd)==-1)
	{
	  Ifile ifi;
	  ifi.setFd(newfd);
	  ifi.setOldFd(oldfd);
	  ifi.setName(std::string("UNKNOWN-DUP3"));
	  ifi.setState(IOBYFILE_DUP3);
	  mtx_iio2.lock();
	  myiio.iiof.push_back(ifi);
	  mtx_iio2.unlock();
	}
      else
	{
	  myiio.getFd(oldfd).setFd(newfd);
	  myiio.getFd(newfd).setState(IOBYFILE_DUP3);
	  myiio.getFd(newfd).setOldFd(oldfd);
	}
    }
  mtx.unlock();
  return retcode;
}

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
  //  printf("bye\n");
}
