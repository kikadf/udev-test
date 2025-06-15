CC = 		gcc
CFLAGS = 	-g -Wall -I/usr/include
LDFLAGS =	-L/lib -L/usr/lib
UNAME := 	$(shell uname)

ifeq ($(UNAME),FreeBSD)
	CC =		clang
	CFLAGS +=	-I/usr/local/include
	LDFLAGS +=	-L/usr/local/lib
endif

ifeq ($(UNAME),OpenBSD)
	CC =	clang
	CFLAGS +=	-I/usr/local/include
	LDFLAGS +=	-L/usr/local/lib
endif

ifeq ($(UNAME),NetBSD)
	CFLAGS +=	-I/usr/pkg/include
	LDFLAGS +=	-L/usr/pkg/lib -R/usr/pkg/lib
endif

utest:
	$(CC) $(CFLAGS) $(LDFLAGS) -o utest utest.c -ludev

all: utest

clean:
	rm -f utest
