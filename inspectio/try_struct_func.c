#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct   ioByFileObj {

  int         state;              // 
  int         current_fd;         // -1 at the beginning // -2 at the end // >=0 
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

  void                       (*noop)(     struct ioByFileChainlist  * self);
  int                        (*size)(     struct ioByFileChainlist  * self);
  void                       (*printList)(struct ioByFileChainlist  * self);
  
  struct ioByFileChainlist * (*new)(      struct ioByFileChainlist  * self);
  struct ioByFileChainlist * (*get)(      struct ioByFileChainlist  * self,int elemNumber);  
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
  struct ioByFileChainlist * iter;
  
  new=prepareIoByFileChainlist();
  while(iter->next!=NULL) 
    { 
      iter=iter->next;
    } 
  iter->next=new;
  return new;
}

struct ioByFileChainlist * getFuncIoByFileChainlist(struct ioByFileChainlist * self,int elemNumber)
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

 void printListIoByFileChainlist(struct ioByFileChainlist * self) 
 { 
   struct ioByFileChainlist * iter=self; 
   printf("printing\n"); 

   while(iter!=NULL) 
   { 
     printf("[%d] [%s]\n",iter->iobfo->current_fd,iter->iobfo->tab_fd_to_name); 
     iter=iter->next; 
   } 
 } 

struct ioByFileChainlist * prepareIoByFileChainlist()
{
  struct ioByFileChainlist * self;

  printf("prepare\n");
  self=malloc(sizeof(struct ioByFileChainlist));


  self->iobfo      = malloc(sizeof(struct ioByFileObj));
  self->next       = NULL;
  self->noop       = noopFuncIoByFileChainlist;
  self->printList  = printListIoByFileChainlist;
  self->size       = sizeFuncIoByFileChainlist;
  self->get        = getFuncIoByFileChainlist;
  self->new        = newFuncIoByFileChainlist;
  
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

  chainlist_elem=chainlist_head->get(chainlist_head,0);
  strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file0");

  chainlist_elem=chainlist_head->new(chainlist_head);
  strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file1");

  chainlist_elem=chainlist_head->new(chainlist_head);
  strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file2");
  chainlist_elem->iobfo->current_fd=4;
  
  printf("ListSize:%d\n",chainlist_head->size(chainlist_head));
  
  /* chainlist_elem=chainlist_head->get(chainlist_head,0); */
  /* strcpy(chainlist_elem->iobfo->tab_fd_to_name,"file0"); */

  
  chainlist_head->printList(chainlist_head);
  return 0;
}
