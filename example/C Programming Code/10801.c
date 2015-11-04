#include<stdio.h>
int main(void)
{
	int blanks,tabs,newlines;
	int c;
	int done=0;
	int lastchar='\n';

	blanks=0;
	tabs=0;
	newlines=0;

	while(done==0)
	{
		c=getchar();
		
		if(c==' ')
			++blanks;
		if(c=='\t')
			++tabs;
		if(c=='\n')
			++newlines;
		if(c==EOF)
		{
			if(lastchar!='\n')
			{
				++newlines;
			}
		done=1;
		}
	lastchar=c;
	}
	printf("blanks:%d,tabs:%d,newlines:%d\n",blanks,tabs,newlines);
	return 0;
}
