	
/*************************************************************************
    > File Name: byteorder.c
    > Author: ljy
    > Mail: jywk99@gmail.com
    > Created Time: THU 26 Mar 2015 02:03:08 PM CST
 ************************************************************************/

#include<stdio.h>
#include<arpa/inet.h>

int main(void)
{
    unsigned int x = 0x12345678;
	printf("unsigned int x = 0x%x\n",x);
    unsigned char *p = (unsigned char *)&x;
	printf("host byte order:\n");
    printf("%x %x %x %x\n", p[0], p[1], p[2], p[3]);
	//printf("%x %x %x %x\n", *p, *(p+1), *(p+2), *(p+3));

    unsigned int y = htonl(x);
	printf("network byte order:\n");
    p = (unsigned char *)&y;
    printf("%x %x %x %x\n", p[0], p[1], p[2], p[3]);

    return 0;
}
