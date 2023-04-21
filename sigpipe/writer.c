# include <unistd.h>
# include <stdio.h>
# include <signal.h>
# include <string.h>

int writeCount = 0;
void sighandler(int sig) {
    char buf1[30] ;
    sprintf(buf1,"signal %d writeCount %d\n", sig, writeCount);
    ssize_t leng = strlen(buf1);
    write(2, buf1, leng);
    _exit(1);
}

int main()
{
    int i = 0;
    char buf[2] = "a";
    struct sigaction ss;
    ss.sa_handler = sighandler;
    sigaction(13, &ss, NULL);
    while(1)
    {
        ssize_t c = write(1, buf, 1);
        writeCount++;
    }
}

