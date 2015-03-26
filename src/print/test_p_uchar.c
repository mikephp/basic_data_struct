#include <stdio.h>
extern int p_uchar(void* str,int len,int flag);
int main(int argc,char* argv[])
{
	printf("print string one byte by one byte in host environment\n");
	printf("use 2 hexadecimal numbers\n");
	int a[3];int num=3;
	a[0]=0x12345678;
	a[1]=0x90abcdef;
	printf("&a[0] = %p\n",&a[0]);
	printf("&a[1] = %p\n",&a[1]);
	p_uchar(&a,num*sizeof(int),1);
	return 0;
}
