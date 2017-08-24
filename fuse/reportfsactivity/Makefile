
CFLAGS = -Wall -DFUSE_USE_VERSION=26 `pkg-config fuse --cflags`
LINKFLAGS = -Wall `pkg-config fuse --libs`

all: bin/reportfsactivity

clean:
	rm -rf bin obj

bin: 
	mkdir -p bin

bin/reportfsactivity: bin obj/reportfsactivity.o obj/wrap.o obj/main.o
	g++ -g $(LINKFLAGS) -o bin/reportfsactivity obj/*

obj:
	mkdir -p obj

obj/reportfsactivity.o: obj reportfsactivity.cc reportfsactivity.hh
	g++ -g $(CFLAGS) -c reportfsactivity.cc -o obj/reportfsactivity.o

obj/main.o: obj main.c wrap.hh
	g++ -fpermissive -g $(CFLAGS) -c main.c -o obj/main.o

obj/wrap.o: obj wrap.cc wrap.hh reportfsactivity.hh
	g++ -g $(CFLAGS) -c wrap.cc -o obj/wrap.o

