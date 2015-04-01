#include<stdio.h>
#include<arpa/inet.h>
#include<string.h>
#include "hash5.h"
int main()
{
	u32 ip=0xf00f77cc;
	u16 ht=1;
	ht = hash_sip(ip);
	printf("ip=%#-8x,ht=%#x\r\n",ip,ht);
	return 0;
}
