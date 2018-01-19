#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define HOSTINFOSIZE 512

void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

int main()
{
	int rc;
	char txt[HOSTINFOSIZE];
	char txt2[HOSTINFOSIZE];
	rc = gethostname(txt,HOSTINFOSIZE-1);
	printf("%s\n",txt);
	removeSubstring(txt,"-HP-ProBook-650-");
	printf("%s\n",txt);
}
