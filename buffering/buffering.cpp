#include <stdio.h>
#include <stdlib.h> // malloc
#include <stdint.h> // int64_t
#include <string.h> // memcpy
#include <unistd.h> // sleep

#define MALLOC_ERROR 1

class bio
{
public:
  bio();
  ~bio();  
  void puts(char * d,int64_t s,int64_t o);
  void print();
  int  mergeable(bio tomerge);
private:
  int64_t    size;
  int64_t    offset;
  char     * data;  
};


bio::bio()
{
  size=-1;
  offset=-1;
  data=NULL;
}

bio::~bio()
{  
  if (data!=NULL)
    {
      printf("~bio() freeing D:%.16s\n",data);
      free(data);
    } 
}

void bio::print()
{
  if (data)
    printf("O:%.4d S:%.4d D:%.16s\n",offset,size,data);
}

void bio::puts(char *d,int64_t s,int64_t o)
{
  if (size == -1)
    data=(char*)malloc(sizeof(char)*s);
  if (data==NULL)
    {
      perror("bio::puts malloc error\n");
      exit(MALLOC_ERROR);
    }
  memcpy(data,d,s);
  offset=o;
  size=s;
}

int bio::mergeable(bio tomerge)
{
  if (size+offset==tomerge.offset)
    return 1;
  return 0;
}

int main()
{
  bio a;
  bio b;

  a.puts("salut",5,0);
  b.puts("toi",3,5);

  a.print();
  b.print();
  printf("%d\n",a.mergeable(b));
  a.print();
  b.print();
  
  //sleep(1);

}
