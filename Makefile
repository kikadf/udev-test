CC = 		gcc
CFLAGS = 	-g -Wall -I/usr/include -I/usr/local/include
LDFLAGS =	-L/lib -L/usr/lib -L/usr/local/lib
UNAME := 	$(shell uname)

ifeq ($(UNAME),FreeBSD)
	CC =	clang
endif

ifeq ($(UNAME),OpenBSD)
	CC =	clang
endif

udev_test:
	$(CC) $(CFLAGS) $(LDFLAGS) -o udevtest udev_test2.c -ludev

all: udev_test

clean:
	rm -f udevtest
