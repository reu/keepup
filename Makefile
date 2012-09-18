PREFIX?=/usr/local
CFLAGS=-Wall -g

all:
	$(CC) keepup.c -o keepup

install: all
	cp -f keepup $(DESTDIR)$(PREFIX)/bin/keepup

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/keepup

clean:
	rm -f keepup
