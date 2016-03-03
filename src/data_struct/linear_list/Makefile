TOP=..
cc = gcc
BIN=test
FPIC = -fPIC -c

HEADER= -I$(TOP)/common -I.
COMMON_LIBA=libdatastructcommon.a
L=	-L$(TOP)/lib/$(COMMON_LIBA)

all:$(BIN)

$(BIN): test_sequence_list.o sequence_list.o $(TOP)/lib/$(COMMON_LIBA)
	$(cc) $^ -o $@ $(L) $(HEADER)

%.o:%.c
	$(cc) $(FPIC) $< -o $@ $(HEADER) $(L)

clean:
	rm -fr $(BIN) *.o
