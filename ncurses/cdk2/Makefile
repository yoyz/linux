.SUFFIXES:
CC=g++
CFLAGS=-Wall -I. -lncurses -lcdk
LDFLAGS= -lncurses -lcdk
SOURCES=cdk2.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=cdk

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
