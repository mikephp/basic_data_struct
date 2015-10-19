cc=gcc
cflags= -levent
.PHONY:all clean

all:
	$(cc) $(cflags) ser.c -o ser
	$(cc) cli.c -o cli

clean:
	rm -rf ser cli
