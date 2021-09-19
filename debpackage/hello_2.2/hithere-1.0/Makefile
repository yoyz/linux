prefix = /usr/local

all:
	$(CC) -o hithere hithere.c

install:
	install hithere $(DESTDIR)$(prefix)/bin
	install hithere.1 $(DESTDIR)$(prefix)/share/man/man1

clean:
	rm -f hithere
