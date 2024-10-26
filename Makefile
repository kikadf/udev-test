CC = gcc
CFLAGS = -g -Wall
UNAME := 	$(shell uname)

ifeq ($($(UNAME),FreeBSD)
	CC = clang
endif
ifeq ($($(UNAME),OpenBSD)
	CC = clang
endif

udev_test:
	$(CC) $(CFLAGS) -o udev_test udev_test.c -ludev

all: udev_test

clean:
	rm -f udev_test
