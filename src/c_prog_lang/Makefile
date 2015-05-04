SRC=$(wildcard *.c)
BIN=test
OBJ=$(patsubst %.c,%.o,$(SRC))

.PHONY:all
all: $(OBJ) $(BIN)

$(OBJ):
	gcc -fPIC -c $(SRC) 

$(BIN):
	gcc $(OBJ) -o $(BIN)

.PHONY:clean
clean:
	rm $(OBJ) $(BIN)
