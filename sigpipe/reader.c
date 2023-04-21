# include <unistd.h>
# include <stdio.h>

int main() {

    ssize_t n ;
    char a[5];
    n = read(0, a, 3);
    printf("read %zd bytes\n", n);
    return(0);

}
