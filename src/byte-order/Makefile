CC = gcc
CFLAGS = -Wall

FILE_SUFFIX = c

SRCS := $(wildcard *.$(FILE_SUFFIX))
EXES := $(patsubst %.$(FILE_SUFFIX),%,$(SRCS))

.PHONY:all
all:$(EXES)

$(EXES): % : %.$(FILE_SUFFIX)
	$(CC) $(CFLAGS) $< -o $@

.PHONY:all
clean:
	rm -rf $(EXES)
