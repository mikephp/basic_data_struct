#include <stdio.h>

int main(int argc ,char * argv[])
{
	int i;
	printf("\r\n");
	for(i = 0; i < 100; i++)
	{
	
		printf("speed %d",i);
		fflush(stdout);
		sleep(1);
		printf("\r");
	}
	return 0;
}
