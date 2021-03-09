#include <stdint.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>       /* for AF_INET */

int main(int argc, char **argv)
{
   uint32_t addr=0x7e3b820a;  //  126.59.130.224
   //uint32_t addr=0x0101a8c0;    // 1.1.168.192 which is the reverse of 192.168.1.1
   //uint32_t addr=0x08080808;  // 8.8.8.8 google dns
   struct hostent  *ent;
   ent = gethostbyaddr(&addr, sizeof(addr), AF_INET);
   if (!ent) {
           printf( "can't resolve 0x%x\n", addr);
           //return -1;
   }
   else
	   printf("%s\n",ent->h_name);
  
  printf("%d %d %d %d\n",
		  ( addr&0xFF000000)>>24 ,
		  ( addr&0x00FF0000)>>16 ,
		  ( addr&0x0000FF00)>>8  ,
		  ( addr&0x000000FF)>>0  
	);
}
