#include <stdio.h>
int main()
{
	char a[100];
	char b[100]="hello world";
	sscanf(b,"%s",a);
	printf("%s\n",a);
	return 0;
}