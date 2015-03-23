#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
	char buf[128];
	char test[]="abcdef";
	strncpy(buf,test+1,3);
	//strncpy(buf,"tttt",4);
    printf("%s\n",buf);
	return 0;
}
