#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define SIZE 1048576

typedef struct{
	int64_t offset;
	int64_t size;
}PART;

char file_in[512]; 		/* source file name */
char file_out[512]; 		/* target file name */


char *tvalue   = NULL;
int tvalue_int = 1;
char * opt_arg_one;
char * opt_arg_two;
pthread_t * T; 
void* func1(void *arg);
struct stat 	f_stat;
PART * P;

void* copy_part_file(void *arg){
	int fin, fout;
	int64_t i,x;
	PART *part;
	char data[SIZE];
	
	part = (PART *)arg;
	fin  = open(file_in, O_RDONLY);
        
	fout = open(file_out, O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);

	lseek(fin, part->offset, SEEK_SET);
	lseek(fout, part->offset, SEEK_SET);

	//printf("BEGIN offset : %llu part->size : %llu\n",part->offset,part->size);
        while(i < part->size){
	//	printf("i : %llu x : %llu part->size : %llu\n",i,x,part->size);
                x = read(fin, data, SIZE);
                write(fout, data, x);
                i += x;
        }
	//printf("END offset : %llu part->size : %llu\n",part->offset,part->size);
        //printf("thread is done.\n");
        close(fout);
        close(fin);
}

void prepare_copy_one_file()
{
  int i;
  strcpy(file_in,  opt_arg_one);
  strcpy(file_out, opt_arg_two);
  printf("%s %s \n",file_in,file_out);
  stat(file_in, &f_stat); 	// getting the meta info of file
  printf("Size of file is %lu \n", f_stat.st_size);
  P=malloc(sizeof(PART)*tvalue_int);      // allocate space for each part
  T=malloc(sizeof(pthread_t)*tvalue_int); // allocate space for each thread
  for (i=0;i<tvalue_int;i++)
  {
  	P[i].size=f_stat.st_size/tvalue_int;
	P[i].offset=(f_stat.st_size/tvalue_int)*(i);
	printf("thread : %d size : %lld offset : %lld\n",i,P[i].size,P[i].offset);
  }
  for (i=0;i<tvalue_int;i++)     
  {                              
  	pthread_create(&T[i], NULL, copy_part_file, &P[i]);
  }
  for (i=0;i<tvalue_int;i++)     
  {                              
	pthread_join(T[i], NULL); // waiting thread
  }
  printf("file is copied");

}

int main(int argc, char *argv[])
{
  int aflag = 0;
  int bflag = 0;
  int index;
  int c;
 int  optionnal_argument=0;

  opterr = 0;

  while ((c = getopt (argc, argv, "abt:")) != -1)
    switch (c)
      {
      case 'a':
        aflag = 1;
        break;
      case 'b':
        bflag = 1;
        break;
      case 't':
        tvalue = optarg;
	tvalue_int=atoi(tvalue);
        break;
      case '?':
        if (optopt == 't')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  printf ("aflag = %d, bflag = %d, tvalue = %s\n",
          aflag, bflag, tvalue);

  for (index = optind; index < argc; index++)
	{
    		printf ("Non-option argument %s\n", argv[index]);
		optionnal_argument++;
		if (optionnal_argument==1) opt_arg_one=argv[index];
		if (optionnal_argument==2) opt_arg_two=argv[index];
	}
  if (optionnal_argument!=2 ) 
	{
		printf("not a valid number of non optionnal argument %d given, 2 required\n",optionnal_argument);
		exit(1);
	}
  prepare_copy_one_file();
  return 0;
}
