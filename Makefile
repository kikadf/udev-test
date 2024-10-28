CC = 		gcc
CFLAGS = 	-g -Wall -I/usr/include -I/usr/local/include
LDFLAGS =	-L/lib -L/usr/lib -L/usr/local/lib
UNAME := 	$(shell uname)

ifeq ($(UNAME),FreeBSD)
	CC = 	clang
endif

ifeq ($(UNAME),OpenBSD)
	CC = 	clang
endif

udev_test:
	$(CC) $(CFLAGS) $(LDFLAGS) -o udev_test2 udev_test2.c -ludev
	$(CC) $(CFLAGS) $(LDFLAGS) -o udev_test udev_test.c -ludev

all: udev_test

clean:
	rm -f udev_test udev_test2
