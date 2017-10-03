.SUFFIXES:
CC=g++
CFLAGS=-Wall -I. -lncurses
LDFLAGS= -lncurses
SOURCES=hw.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=hw

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
