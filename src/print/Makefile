CC = gcc
CFLAGS = -Wall -O

FILE_SUFFIX = c

SRCS := $(wildcard *.$(FILE_SUFFIX))
EXES := test 

.PHONY:all clean
all:$(EXES)
$(EXES): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(EXES) *.o
