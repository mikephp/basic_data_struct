#include <stdio.h>
#include "common.h"
int p_debug_mask = 0xff;
int p_debug      = 0;

int printc(char* ch,int rn, void *buf)
{
	int i;
	unsigned char *pbuf;
	pbuf = (unsigned char*)buf;
	printf("%s length %d\n",ch, rn);
	for (i = 0; i < rn; i++) {
		if ((i % 8) == 0 && i != 0)
			printf( "  #%d \n",i);
		printf( "%02X ", (*pbuf&0xff));
		pbuf++;
	}
	printf( "\n");
	return 0;
}