# Makefile

# gcc -Wall -fPIC -shared -o inspectio.so inspectio.c -ldl
SRC     = inspectio.c
OBJ     = inspectio.so


CC       = gcc
CFLAGS   = -Wall -Wunused-variable -fPIC -shared -ldl
CFLAGS   = -fPIC -shared -ldl
LDLIBS   =

all: ${OBJ}

${OBJ}: ${SRC}
	${CC} ${CFLAGS} -o ${OBJ}  ${SRC} 

clean:
	rm -f $(BIN) $(OBJ)
