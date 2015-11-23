#include<stdio.h>
void add(int a[]){
	int i;
	for(i = 0 ; i < 3; i++)
	{
		a[i] = a[i] + i;	
	}
}
void main()
{
	int i,b[3]={0,1,2};

	//for(i = 0 ; i < 3; i++){printf("b[%d] = %d\n",i,b[i]);}
	add(b);
	for(i = 0 ; i < 3; i++){printf("b[%d] = %d\n",i,b[i]);}

}
