srcdir=.
top_srcdir=..
top_builddir=..

COMMONOBJ  = SS5OpenLdap.o SS5Radius.o SS5MySql.o


CFLAGS=-g -O2 -DLINUX -D_FILE_OFFSET_BITS=64 -I . -I ../include   -ggdb -Wall -Wextra -pedantic -fno-strict-aliasing  -fPIC
SRVCFLAGS=-g -O2 -DLINUX -D_FILE_OFFSET_BITS=64 -I . -I ../include  -ggdb -Wall -Wextra -pedantic -fno-strict-aliasing 

LIBS = -lpthread -rdynamic -ldl -lssl -lldap -lpam -lpam_misc -lcrypto  

CC = gcc

all:	$(COMMONOBJ)


clean:
	rm -f $(COMMONOBJ)

distclean: clean
	rm -f Makefile
