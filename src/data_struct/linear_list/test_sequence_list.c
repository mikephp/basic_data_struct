#include <stdio.h>
#include "sequence_list.h"
#include "common.h"

int main(int argc,char*argv[])
{
	SqList list;
	List_Init(&list);
	printf("list length %d\n",list.length);
	EMTY a[5];
	a[0]=1;
	a[1]=2;
	a[2]=3;
	List_Insert(&list,0,&a[0]);
	List_Insert(&list,1,&a[1]);
	List_Insert(&list,2,&a[2]);
	printf("list length %d\n",list.length);
	List_Print(&list);
	return 0;
}
