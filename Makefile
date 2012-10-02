PREFIX?=/usr/local
CFLAGS=-Wall -g

all:
	$(CC) keepup.c -o keepup

install: all
	cp -f keepup $(PREFIX)/bin/keepup

uninstall:
	rm -f $(PREFIX)/bin/keepup

clean:
	rm -f keepup
