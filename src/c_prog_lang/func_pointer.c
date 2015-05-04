#include <stdio.h>
#include <string.h>

void qsort(void *lineptr[], int left, int right,
		  int (*comp)(void *, void *));
int numcmp1(char *, char *);
int numcmp2(char *, char *);
/* sort input lines */
main(int argc, char *argv[])
{
	(int (*)(void*,void*))(1 ? numcmp1: numcmp2);
	return 0;
}
int numcmp1(char *, char *)
{
	printf("method 1\r\n");
}
int numcmp2(char *, char *)
{
	printf("method 2\r\n");
}