#include<stdio.h>
//#include<inttype.h>
#include<unistd.h>

static union { 
	char c[4]; 
	unsigned long mylong; 
} endian_test = {{ 'l', '?', '?', 'b' }	};

#define ENDIANNESS ((char)endian_test.mylong)

int main()
{
	printf("%c\n",ENDIANNESS);

	int	mask = 112;
	unsigned int a = 0xffffffff;
	unsigned int b = 0xffffffff;
	int v=mask/64;
	int r=mask%64;
	a = (a<<32);
	//b = (b<<31);
	printf("a<<32 =%08x\n",a);
	printf("b<<31 =%08x\n",b<<31);
	printf("b 	  =%08x\n",b);
	printf("b<<32 =%08x\n",b<<32);
	printf("b>>32 =%08x\n",b>>32);
	printf("b 	  =%08x\n",b);
	printf("b>>31 =%08x\n",b>>31);
	printf("b	  =%08x\n",b);
	int c=1;
	printf("%08x\n",c);
	printf("~c=%08x\n",~c);
	printf("!c=%08x\n",!c);
	printf("~c<<32=%08x\n",~c<<32);
	printf("~c<<31=%08x\n",~c<<31);
	return 0;
}

