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
  int init(int64_t s);
  int fini();
  int puts(char * d,int64_t s,int64_t o);
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
}

void bio::print()
{
  if (data)
    printf("O:%.4d S:%.4d D:%.16s\n",offset,size,data);
}

int bio::init(int64_t s)
{
  if (data==NULL)
    {
      data=(char*)malloc(sizeof(char)*s);
    }
  else
    {
      printf("Recycling\n");
      fini();
      data=(char*)malloc(sizeof(char)*s);
    }
  if (data)
    {
      size=s;
      return s;
    }
  else
    return -1;
}
    

int bio::fini()
{
  if (data)
    free(data);
  size=-1;
  offset=-1;
}


int bio::puts(char *d,int64_t s,int64_t o)
{
  if (this->size == -1)
    this->init(s);
  
  if (this->size != s)
    {
      this->fini();
      this->init(s);
    }
  
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

 
class biolist
{
public:
  biolist();
  int insert(bio toinsert);
  void print();
};
 
biolist::biolist()
{
  
}

int biolist::insert(bio toinsert)
{

}

void biolist::print()
{
  printf("biolist\n");
}

 
int main()
{
  bio a;
  bio b;

  biolist bl;

  char salut[] = "salut";
  char toi[]   = "toi";
  
  a.puts(salut,strlen(salut),0);
  b.puts(toi,strlen(toi),5);

  a.print();
  b.print();
  printf("%d\n",a.mergeable(b));
  a.fini();
  a.print();
  b.print();

  bl.print();
  
  
  //sleep(1);

}
