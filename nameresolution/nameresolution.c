#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

int resolveHostname(char * hostname , char* ip)
{
  struct hostent *hent;
  struct in_addr **addr_list;
  int i;
  
  if (!(hent = gethostbyname( hostname )))
    {
      herror("gethostbyname error");
      return 1;
    }
  addr_list = (struct in_addr **) hent->h_addr_list;
  
  for(i = 0; addr_list[i] != NULL; i++)
    strcpy(ip , inet_ntoa(*addr_list[i]));
  
  return 0;
}
 
int main(int argc , char *argv[])
{
  char *hostname = argv[1];
  char ip[100];     resolveHost(hostname,ip);

  if(argc <2)
    {
      printf("No hostname is given to resolve");
      exit(1);
    }
  else
    {
      printf("%s resolved to %s\n" , hostname , ip);
    }
  return 0;
}

