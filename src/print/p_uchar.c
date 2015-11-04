#include <stdio.h>
/*
*	
*/
/*int p_uchar(void* str,int len,int flag)
{
	int i;
	unsigned char *p;
	p=(unsigned char*) str;
	printf("\n\n");
	for(i=0;i<len;i++){
		if(flag){printf("%p=%02x ",p,*p);p++;}
		else printf("%02x ",*p++);
		if(0==(i+1)%4)printf("\n");
	}
	printf("\n\n");
	return i;
}*/
int p_uchar(void* str,int len,int flag)
{
	int i;
	unsigned char *p;
	p=(unsigned char*) str;
	printf("\n\n");
	for(i=0;i<len;i++){
		if(1==flag){
			printf("%p=%02x ",p,*p);
			p++;
			if(0==(i+1)%4)printf("\n");
		}
		else{
			printf("%02X ",*p++);
			if(0==(i+1)%32)printf("\n");
		}
	}
	printf("\n\n");
	return i;

