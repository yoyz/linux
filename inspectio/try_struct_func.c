#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
  void                       (*printList)(   struct ioByFileChainlist  * self);
  
  struct ioByFileChainlist * (*new)        ( struct ioByFileChainlist  * self);
  struct ioByFileChainlist * (*getByNumber)( struct ioByFileChainlist  * self,int elemNumber);
  struct ioByFileChainlist * (*getByFd)    ( struct ioByFileChainlist  * self,int fd);

  
  
};

struct ioByFileChainlist * prepareIoByFileChainlist();


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
       if (iter->iobfo->fd==fd)
	 return iter;
       iter=iter->next;
     }
   return NULL;
}


 void printListIoByFileChainlist(struct ioByFileChainlist * self) 
 { 
   struct ioByFileChainlist * iter=self;
   char * state_new   = "new";
   char * state_open  = "open";
   char * state_read  = "read";
   char * state_write = "write";
   char * state_close = "close";
   char * state_seek =  "seek";
   char state[8];
   
   printf("printing\n"); 

   while(iter!=NULL) 
   {
     switch (iter->iobfo->state)
       {
       case IOBYFILE_NEW  :	 strcpy(state,state_new); 	 break;
       case IOBYFILE_OPEN :	 strcpy(state,state_open); 	 break;
       }
	 printf("fd:%5.1d state:%6.6s filename:%32.32s\n",
		iter->iobfo->fd,
		state,
		iter->iobfo->tab_fd_to_name);
	 
     iter=iter->next; 
   } 
 } 

struct ioByFileChainlist * prepareIoByFileChainlist()
{
  struct ioByFileChainlist * self;

  printf("prepare\n");
  self=malloc(sizeof(struct ioByFileChainlist));


  self->iobfo        = malloc(sizeof(struct ioByFileObj));

  self->iobfo->state = IOBYFILE_NEW;
  self->iobfo->fd    = -1;
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



int main(int argc, char **argv)
{
  struct ioByFileChainlist * chainlist_head;
  struct ioByFileChainlist * chainlist_elem;


  chainlist_head = prepareIoByFileChainlist();
  chainlist_head->size(chainlist_head);
  chainlist_head->noop(chainlist_head);
  chainlist_head->printList(chainlist_head);

  printf("ListSize:%d\n",chainlist_head->size(chainlist_head));
  //chainlist_head->printList(chainlist_head);

  chainlist_elem=chainlist_head->getByNumber(chainlist_head,0);
  strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file0");
  chainlist_elem->iobfo->fd=-1;

  chainlist_elem=chainlist_head->new(chainlist_head);
  strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file1");
  chainlist_elem->iobfo->fd=-4;
  chainlist_elem->iobfo->state=IOBYFILE_OPEN;
  
  chainlist_elem=chainlist_head->new(chainlist_head);
  strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file2");
  chainlist_elem->iobfo->fd=4;
  
  printf("ListSize:%d\n",chainlist_head->size(chainlist_head));


  //Find an element
  chainlist_elem=chainlist_head->getByFd(chainlist_head,-1);
  if (chainlist_elem!=NULL)
    printf("    Found : %d\n",chainlist_elem->iobfo->fd);
  else
    printf("Not Found...\n");

  
  chainlist_head->printList(chainlist_head);
  return 0;
}
