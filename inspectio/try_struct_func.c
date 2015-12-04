#include <stdio.h>
#include <stdlib.h>

typedef struct   iobyfile {

  int         state;              // 
  //int         current_fd;         // -1 at the beginning // -2 at the end // >=0 
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



typedef struct fd_chainlist
{
  int                     fd;
  iobyfile_t            * iobf;
  struct   fd_chainlist * next;
} fd_chainlist_t;





void print_list(fd_chainlist_t * head)
{
    fd_chainlist_t * current = head;

    while (current != NULL)
      {
        printf("%d\n", current->fd);
        current = current->next;
      }
}
void push_end(fd_chainlist_t * head, int fd)
{
    fd_chainlist_t * current = head;
    while (current->next != NULL)
      {
        current = current->next;
      }

    /* now we can add a new variable */
    current->next = malloc(sizeof(fd_chainlist_t));
    current->next->fd = fd;
    current->next->next = NULL;
}

void push(fd_chainlist_t ** head, int fd)
{
    fd_chainlist_t * new_node;
    new_node = malloc(sizeof(fd_chainlist_t));

    new_node->fd = fd;
    new_node->next = *head;
    *head = new_node;
}


int pop(fd_chainlist_t ** head)
{
    int retval = -1;
    fd_chainlist_t * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->fd;
    free(*head);
    *head = next_node;

    return retval;
}

int remove_last(fd_chainlist_t * head)
{
    int retval = 0;
    /* if there is only one item in the list, remove it */
    if (head->next == NULL)
      {
	head->fd;
	free(head);
	head = NULL;
	//return fd;
      }

    fd_chainlist_t * current = head;

    while (current->next->next != NULL)
      {
        current = current->next;
      }
}

int remove_by_index(fd_chainlist_t ** head, int n) {
    int i = 0;
    int retval = -1;
    fd_chainlist_t * current = *head;
    fd_chainlist_t * temp_node = NULL;
    //int i;

    if (n == 0)
      {
        return pop(head);
      }

    for (i = 0; i < n-1; i++)
      {
        if (current->next == NULL)
	  {
            return -1;
	  }
        current = current->next;
      }

    temp_node = current->next;
    retval = temp_node->fd;
    current->next = temp_node->next;
    free(temp_node);

    return retval;

}

fd_chainlist_t * create_node()
{
  fd_chainlist_t * head = NULL;
  head = malloc(sizeof(fd_chainlist_t));
  if (head == NULL)
    {
      return NULL;
    }
  
  head->fd = -1;
  head->next = NULL;
  return head;
}


int main(int argc, char **argv)
{
  printf("bla\n");

  iobyfile_t bla;
  fd_chainlist_t list;
  list.iobf->state=1;
  
  
  return 0;
}
