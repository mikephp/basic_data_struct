#include<stdio.h>
   extern char *month_name(int n);
int main()
	  {
	   int i=0;
	char *p;
	while(NULL!=month_name(i)){
		p = month_name(i);
		//printf("%s\r\n",p);
		i++;
		printf("poniter=%p,string=%s\r\n",p);
		}
	return 0;
	}
