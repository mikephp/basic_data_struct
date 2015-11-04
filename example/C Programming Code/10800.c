//Í³¼Æ¿Õ¸ñ¡¢ÖÆ±í·û¡Óë»»ÐÐ·ûµÄ¸öÊý
#include<stdio.h>
int main(void)
{
	int c;
	long blanks,tabs,newlines;
	int lastchar='\n';

	blanks=0;
	tabs=0;
	newlines=0;

	while((c=getchar())!=EOF)
	{
		if(c==' ')
			++blanks;
		if(c=='\t')
			++tabs;
		if(c=='\n')
			++newlines;
		lastchar=c;
	}
	if(lastchar!='\n')
		++newlines;
	printf("blanks:%ld\n",blanks);
	printf("tabs:%ld\n",tabs);
	printf("newlines:%ld\n",newlines);
	return 0;
}
