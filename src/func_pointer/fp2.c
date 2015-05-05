#include <string.h>
#include <stdio.h>
typedef int* PINNT;
#define PP int*

int funcA(int a,int b);
int funcB(int* a,int *b);
int  main(int argc, char *argv[])
{
	int (*func)(int,int);
	func = &funcA;
    //func = funcA;
	//两种赋值给函数指针的方法都可以
	printf("func->funcA,%d\r\n"\
		"func=%p\r\n&func=%p\r\nfuncA=%p\r\n&funcA=%p\r\n", \
		func(1,10),func,&func,funcA,&funcA);
	//func = funcB;
	//printf("func->funcB,%d\r\n",func(1,10));
	//printf("%d",(*func)(1,10));
	//两种调用函数指针的方法都可以
	//两种赋值方法和两种调用方法可以任选一种组合
	
}

int funcA(int a,int b)
{
	return a + b;
}

int funcB(int* a,int *b)
{
	(*a) = (*a) + (*b);
	return 1;
}