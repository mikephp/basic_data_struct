#include<stdio.h>
#include<string.h>
union A
{
	int a;
	char b;
	char c[9];
};
struct B
{
	int a;
	char b;
	char c[9];
};
union C
{
	int a:4;
	char b;
	char c[9];
};

struct D
{
	int a;
	char b;
	char c[19];
};
struct E
{
	int a;
	char b;
	char c[29];
};

int square(volatile int *ptr )
{
	return *ptr * *ptr;
}
int main(int argc ,char* argv[])
{
	union A a;
	struct B b;
	union C c;
	struct D d;
	struct E e;
	volatile int *p;
	int i=10;
	p=&i;
	volatile int *ptr;
	*ptr = 11;
	
	printf("sizeof(A)=%d\r\n",sizeof(a));
	printf("sizeof(B)=%d\r\n",sizeof(b));
	printf("sizeof(C)=%d\r\n",sizeof(c));
	printf("sizeof(D)=%d\r\n",sizeof(d));
	printf("sizeof(E)=%d\r\n",sizeof(e));
	printf("sizeof(int)=%d\r\n",sizeof(int));
	printf("sizeof(short int)=%d\r\n",sizeof(short int));
	printf("sizeof(char *)=%d\r\n",sizeof(char *));
	printf("sizeof(volatile int *p)=%d\r\n",square(p));
	printf("sizeof(volatile int *ptr)=%d\r\n",square(ptr));
	return 0;
}
