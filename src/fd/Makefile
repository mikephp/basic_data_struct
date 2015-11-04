SRCS := $(wildcard *.c)
BIN := $(patsubst %.c,%, $(SRCS))
all:
	gcc $(SRCS) -g  -o $(BIN)
clean:
	rm $(BIN) *.o
