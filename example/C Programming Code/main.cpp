#include<stdio.h>
#define MAXLINE 10	/*���������е���󳤶�*/

int getline(char line[],int maxline);
void copy(char to[],char from[]);

/*��ӡ���������*/

int main()
{
	int len;		//��ǰ�г���
	int max;		//ĿǰΪֹ���ֵ���еĳ���
	char line[MAXLINE];		//��ǰ��������
	char longest[MAXLINE];		//���ڱ��������

	max=0;
	while((len=getline(line,MAXLINE))>0)
		if(len>max)
		{
			max=len;
			copy(longest,line);
		}
	if(max>0)		//������������
		printf("%s",longest);
	return 0;
}

int getline(char s[],int lim)
{
	int c,i;

	for(i=0;i<lim-1&&(c=getchar())!=EOF&&c!='\n';++i)
		s[i]=c;
	if(c=='\n')
	{
		s[i]=c;
		++i;
	}
	s[i]='\0';
	return i;
}

void copy(char to[],char from[])
{
	int i;
	
	i=0;
	while((to[i]=from[i])!='\0')
		++i;
}