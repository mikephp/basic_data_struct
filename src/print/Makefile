CC = gcc
CFLAGS = -Wall -O

FILE_SUFFIX = c

SRCS := $(wildcard *.$(FILE_SUFFIX))
EXES := test 

.PHONY:all
all:$(EXES)
$(EXES): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY:clean
clean:
	rm -rf $(EXES) *.o
