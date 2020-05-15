#include <sys/types.h>
#include <unistd.h>

int main()
{
setuid(0);
printf("getuid:%d\n",getuid());
execve("/bin/bash",NULL,NULL);
}

