#include<stdio.h>

static union { 
	char c[4]; 
	unsigned long mylong; 
} endian_test = {{ 'l', '?', '?', 'b' }	};

#define ENDIANNESS ((char)endian_test.mylong)

int main()
{
	printf("%c\n",ENDIANNESS);
	return 0;
}
