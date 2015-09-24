#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h> 
#include <stdlib.h>
#include <malloc.h>   // for memalign
#include <pthread.h>  // for pthread_create
#include <string.h>   // for strcpy
#include <getopt.h>   // for getopt_long

#define ALIGNED 512

struct operation
{
  int    op; // 0 read, 1 write
  char * filename;
  int    size;
};



uint64_t  * output_buffer=0;
int         firstwrite=0;
void      * tmp;
//char     * filename;
char     ** filenames;
int         filename_number=0; 
int         filename_set=0;
int         time_to_loop;

char      * tmp_char_star;
long int    time_to_wait=10;
int         op_read=0;
int         op_write=0;
int         nb_thread=1;
int         seeknone=0;

int         size_in_byte_to_operate=1024*1024;
int         size_in_byte_set=0;

int         thread_need_to_finish=0;

pthread_t * pth_threads;

operation * op_threads;


void display_help()
{
  printf("Need filename to output\n");
  printf("./directio [-byte <bytes>] [-op <op>] [-seq <seq>] [-threads <thread>] [-time <time>] [--filename <filename>] \n");
  printf("op:       0 write,1 read                         \n");  // 1
  printf("byte:     number of byte multiple of 512         \n");  // 2
  printf("seq:      0 seq,1 loop                           \n");  // 2
  printf("thread:   nb thread                              \n");  // 3
  printf("time  :   time in second                         \n");  // 4
  printf("filename: the target filename                    \n");  // 5     
}


int stick_this_thread_to_core(int core_id)
{
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  if (core_id < 0 || core_id >= num_cores)
    return -1;
  
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);
  
  pthread_t current_thread = pthread_self();
  return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}

void *operation_thread(void *void_ptr)
{
  int        fd;
  int        firstwrite=0;
  ssize_t    output_written=0;

  stick_this_thread_to_core(1);
  fd=open(filenames[0],O_DIRECT|O_RDWR|O_CREAT,S_IRWXU);
  if (!(fd>0))
    { printf("can not open filename open return : %d\n",fd); exit(1); }

  while(!thread_need_to_finish)
    {
      if (firstwrite==0)
	lseek(fd,0,SEEK_SET);

      if (seeknone)
	lseek(fd,0,SEEK_SET);

      output_written=write(fd,output_buffer,(sizeof(uint64_t)/8)*size_in_byte_to_operate);
      if(output_written<=0)
	{ printf("can not write to %s : %d\n",filenames[0],output_written); exit(1); }
      if (firstwrite==0)
	{
	  printf("Writing to %s\n",filenames[0]);
	  firstwrite=1;
	}
    }
  close(fd);
}

int main(int argc, char **argv)
{
  int i;
  int c;

  const  char   * short_opt  = "lhf:t:b:s:";
  struct option   long_opt[] =
    {
      {"help",          no_argument,       NULL, 'h'},
      {"byte",          required_argument, NULL, 'b'},
      {"file",          required_argument, NULL, 'f'},
      {"seeknone",      optional_argument, NULL, 'l'},
      {"threads",       optional_argument, NULL, 't'},
      {"runtime",       optional_argument, NULL, 's'},
      {"operation",     required_argument, NULL, 'o'},
      {NULL,            0,                 NULL, 0  }
    };

  int option_index=0;

  filenames=(char**)malloc(sizeof(char**)*1024);
  pth_threads=(pthread_t*)malloc(sizeof(pthread_t*)*1024);

  while((c = getopt_long(argc, argv, short_opt, long_opt, &option_index)) != -1)
    {
      switch(c)
	{
	case -1:       /* no more arguments */
	case  0:       /* long options toggles */
	  break;

	case 'o':
	  if (strcmp(optarg,"read"));
	  op_read=1;
	  if (strcmp(optarg,"write"));
	  op_write=1;
	  if ((op_write|op_read))
	    {
	      printf("operation need to be write|read\n");
	      exit(1);
	    }
	  break;

	case 'l':
	  seeknone=1;
	  break;

	  
	case 'b':
	  size_in_byte_to_operate=strtol(optarg,&tmp_char_star,0);
	  if (*tmp_char_star != '\0') { printf("size_in_byte_to_operate is not a number\n"); exit(1); }
	  size_in_byte_set=1;
	  break;
	  

	case 's':
	  time_to_wait=strtol(optarg,&tmp_char_star,0);
	  if (*tmp_char_star != '\0') { printf("time is not a number\n"); exit(1); }
	  break;
	  
	case 'f':
	  printf("F : %d %d \n",argc,optind);
	  i=optind-1;
	  while (i < argc && 
		 *argv[i] != '-')
	    {
	      printf("selected file \"%s\"\n", argv[i]);
	      filenames[filename_number]=(char*)malloc(sizeof(char*)*1024);
	      strcpy(filenames[filename_number],argv[i]);
	      filename_number++;
	      i++;
	      filename_set=1;
	    }
	  break;	  

	case 'h':
	  display_help();
	  exit(0);
	  break;

	case 't':
	  nb_thread=strtol(optarg,&tmp_char_star,0);
	  if (*tmp_char_star != '\0') { printf("thread is not a number\n"); exit(1); }
	  break;
	  
	case '?':
	  fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
	  return(-2);
	  
	default:
	  fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
	  fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
	  return(-2);
	};
    };
  
  if (!filename_set) 
    { printf("filename is a mandatory parameter\n"); exit(1); }

  if (!size_in_byte_set) 
    { printf("byte is a mandatory parameter\n"); exit(1); }
  
  output_buffer=(uint64_t*)memalign(ALIGNED,(sizeof(uint64_t)/8)*size_in_byte_to_operate);
  
  if(!output_buffer)
    { printf("can not allocate aligned memory : %d\n",output_buffer); exit(1); }


  printf("Launching %d thread for %d second\n",nb_thread,time_to_wait);

  for (i=1;i<=nb_thread;i++)
    { pthread_create(&pth_threads[i],NULL,operation_thread,tmp); }

  sleep(time_to_wait);
  thread_need_to_finish=1;

  for (i=1;i<=nb_thread;i++)
  { pthread_join(pth_threads[i],NULL);                           }

  printf("All thread join\n");

  return 0;
}
