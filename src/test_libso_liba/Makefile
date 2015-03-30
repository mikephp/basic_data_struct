#*****************************************************************************
# Copyright        :  @mikephp
# Author           :   mikephp
# Date             :   2015-03-30
# Version          : 1.0.0
# Description      : 动态链接库,静态链接库 示例
#****************************************************************************/
PWD = $(shell pwd)
SHELL = /bin/sh
LIB_DIR = $(PWD)/lib/
LIBA_DIR = $(PWD)/liba/
BIN_DIR = $(PWD)/bin/
OBJECT_DIR = $(PWD)/obj/
APP_DIR = $(PWD)/
$(shell mkdir -p ${LIB_DIR})
$(shell mkdir -p ${BIN_DIR})
$(shell mkdir -p ${OBJECT_DIR})
$(shell mkdir -p ${LIBA_DIR})
RM = rm -fr
#****************************************************************************

CC = gcc
SHARED = -shared -o
FPIC = -fPIC -c 
CFLAG=-Wall
SRC_OBJECT = $(APP_DIR)test_a.c $(APP_DIR)test_b.c $(APP_DIR)test_c.c
H_OBJECT = $(APP_DIR)so_test.h
OBJECT = test_a.o test_b.o test_c.o
DY_SRC_OBJECT = $(APP_DIR)test.c
DY_OBJECT=test.o
LIB_OBJECT = libtest.so
LIBA_OBJECT = libtest.a
BIN_OBJECT = test
BINA_OBJECT = testa

#****************************************************************************
#	$(CC) $(FPIC) $(SRC_OBJECT)

.PHONY:all
all:$(LIB_OBJECT) $(BIN_OBJECT) $(OBJECT) $(LIBA_OBJECT)  $(BINA_OBJECT)

$(OBJECT):%.o:$(SRC_OBJECT)
	$(CC) $(CFLAG) $(FPIC) $^ 

$(LIB_OBJECT):$(OBJECT)
	$(CC) $(OBJECT) $(SHARED) -fPIC -o $(LIB_OBJECT)
	mv $(LIB_OBJECT) $(LIB_DIR)

$(LIBA_OBJECT):$(OBJECT)
	ar cqs  $(LIBA_OBJECT) $(OBJECT)
	mv $(LIBA_OBJECT) $(LIBA_DIR)
	mv $(OBJECT) $(OBJECT_DIR)
	
$(BIN_OBJECT):$(DY_OBJECT)
	$(CC) $(OBJECT_DIR)$(DY_OBJECT) -L$(LIB_DIR) -ltest -o $(BIN_OBJECT)
	mv $(BIN_OBJECT) $(BIN_DIR)

$(BINA_OBJECT):$(DY_OBJECT)
	$(CC) $(OBJECT_DIR)$(DY_OBJECT) $(LIBA_DIR)libtest.a -o $(BINA_OBJECT)
	mv $(BINA_OBJECT) $(BIN_DIR)

$(DY_OBJECT):$(DY_SRC_OBJECT)
	$(CC) -c $(DY_SRC_OBJECT)
	mv  $(DY_OBJECT) $(OBJECT_DIR)

.PHONY:clean 
clean:
	$(RM) $(LIB_DIR) $(BIN_DIR) $(OBJECT_DIR) $(LIBA_DIR) *.o
