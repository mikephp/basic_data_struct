cc=g++
SERVER=server
TESTER=tester

.PHONY:all clean

all:$(SERVER) $(TESTER)
$(SERVER):server.o
	$(cc) $^ -o $@

$(TESTER):tester.o
	$(cc) $^ -o $@
%.o:%.c
	$(cc) -c $^ -o $@
clean:	
	rm -fr *.o $(SERVER) $(TESTER)
