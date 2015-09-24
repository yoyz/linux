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


uint64_t * output_buffer=0;
int        firstwrite=0;
void     * tmp;
char     * filename;
int        time_to_loop;

char     * tmp_char_star;
long int   time_to_wait=10;
int        op_read=0;
int        op_write=0;


void display_help()
{
  printf("Need filename to output\n");
  printf("./directio [-op <op>] [-seq <seq>] [-threads <thread>] [-time <time>] [--filename <filename>] \n");
  printf("op:       0 write,1 read                         \n");  // 1
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

void *write_one_mib(void *void_ptr)
{
  int        fd;
  int        firstwrite=0;
  ssize_t    output_written=0;

  stick_this_thread_to_core(1);
  fd=open(filename,O_DIRECT|O_RDWR|O_CREAT,S_IRWXU);
  if (!(fd>0))
    { printf("can not open filename open return : %d\n",fd); exit(1); }

  while(1)
    {
      lseek(fd,0,SEEK_SET);
      output_written=write(fd,output_buffer,(sizeof(uint64_t)/8)*1024*1024);
      if(output_written<=0)
	{ printf("can not write to %s : %d\n",filename,output_written); exit(1); }
      if (firstwrite==0)
	{
	  printf("Writing to %s\n",filename);
	  firstwrite=1;
	}
    }
  close(fd);
}

int main(int argc, char **argv)
{

  int c;
  const  char   * short_opt  = "hf:";
  struct option   long_opt[] =
    {
      {"help",          no_argument,       NULL, 'h'},
      {"file",          required_argument, NULL, 'f'},
      {"threads",       optional_argument, NULL, 't'},
      {"runtime",       optional_argument, NULL, 's'},
      {"operation",     required_argument, NULL, 'o'},
      {NULL,            0,                 NULL, 0  }
    };

  while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1)
    {
      switch(c)
	{
	case -1:       /* no more arguments */
	case 0:        /* long options toggles */
	  break;

	case 'o':
	  if (strcmp(optarg,"read"));
	  op_read=1;
	  if (strcmp(optarg,"write"));
	  op_write=1;
	  if ((op_write|op_read)
		  
	  break;
	  
	case 's':
	  time_to_wait=strtol(optarg,&tmp_char_star,0);
	  if (*tmp_char_star != '\0') { printf("time is not a number\n"); exit(1); }
	  exit(1);
	  break;
	  
	case 'f':
	  printf("selected file \"%s\"\n", optarg);
	  filename=(char*)malloc(sizeof(char*)*1024);
	  strcpy(filename,optarg);
	  break;	  

	case 'h':
	  display_help();
	  exit(0);
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
  
  // else
  //   {

  //     //time handling
  //     time_to_wait=strtol(argv[4],&tmp_char_star,0);
  //     if (*tmp_char_star != '\0') { printf("time is not a number\n"); exit(1); }
  //     //filename handling
  //     filename=(char*)malloc(sizeof(char*)*1024);
  //     strcpy(filename,argv[5]);
  //   }

  //exit(1);
  
  output_buffer=(uint64_t*)memalign(ALIGNED,(sizeof(uint64_t)/8)*1024*1024);
  
  if(!output_buffer)
    { printf("can not allocate aligned memory : %d\n",output_buffer); exit(1); }


  pthread_t pth_thread1;
  pthread_t pth_thread2;
  
  //while(1)
  //{
  printf("Launching thread for %d second\n",time_to_wait);
  pthread_create(&pth_thread1,NULL,write_one_mib,tmp);
  pthread_create(&pth_thread2,NULL,write_one_mib,tmp);
  //}
  sleep(time_to_wait);
  return 0;
}
