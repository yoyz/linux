#define _GNU_SOURCE 1
#include <dlfcn.h>
#include <stdio.h>

static FILE* (*libc_fopen)(char const *, char const *);

char filebuffer[1024][1024*1024];

FILE * fopen(char const *name, char const *mode) {
    FILE *ret;
    int filenumber;
    if (!libc_fopen)
        libc_fopen = dlsym(RTLD_NEXT, "fopen");

    ret = libc_fopen(name, mode);
    if (!ret)
        return ret;

    filenumber=fileno(ret);
    //setvbuf(ret, NULL, _IONBF, 0);
    setvbuf(ret, filebuffer[filenumber], _IOFBF, 1024*1024);

    return ret;
}
