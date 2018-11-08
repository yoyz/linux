#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

int main()
{
	char* buf;
	while ((buf = readline(">> ")) != 0) {
		if (strlen(buf) > 0) {
			add_history(buf);
    			printf( "%s\n", readline( ">> " ) );
			}
		}
    return 0;
}
