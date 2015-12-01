cc=gcc
cflag=-g
flex=flex
yyac=bison
target=cli
.PHONY:all clean

all:$(target)

%.lex.c:%.l
	@echo $@ $^
	$(flex) $^
%.tab.c:%.y
	@echo $@ $^
	$(yyac) -d -v $^

$(target):%.c %.h
	$(cc) $(cflag) $< -o $(target)
	
