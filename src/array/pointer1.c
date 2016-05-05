#include "stdio.h"

int main()
{
    char *p1,*p2,*p3;
	char ch[] = {'a', 'b', 'c'};
	char **pp;
	p1 = ch;
	pp = &ch;
	p2 = *pp;
	printf("char *p1,*p2,*p3;\nchar ch[] = {'a', 'b', 'c'};\nchar **pp;\np1 = ch;\npp = &ch;\np2 = *pp;\n\n");
	printf("p1 %p\np2 %p\npp %p\n ch %p\n&ch %p\n",p1,p2,pp,ch,&ch);
	printf("\n&ch[0] %p\n&ch[1] %p\n&ch[2] %p\n",&ch[0],&ch[1],&ch[2]);
	if (p1 == p2) {
		printf("p1  == p2\n");
	} else {
		printf("p1 != p2\n");
	}

	printf("p3 = %p\n", p3);
	return 0;
}
