TOP=..
cc = gcc
BIN=test
FPIC = -fPIC -c

HEADER= -I$(TOP)/common -I.
COMMON_LIBA=libdatastructcommon.a
LIB=lib
$(shell mkdir -p $(TOP)/$(LIB))
all:$(COMMON_LIBA)

%.o:%.c
	$(cc) $(FPIC) -c $< -o $@ $(HEADER)

$(COMMON_LIBA): common.o
	ar cqs $@ $^
	mv $(COMMON_LIBA) $(TOP)/lib
clean:
	rm -fr $(TOP)/lib/$(COMMON_LIBA) *.o
