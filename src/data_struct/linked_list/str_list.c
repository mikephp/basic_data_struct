#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#define MAX_STR_LEN 2000

typedef struct List
{
int id;
char *data;
struct List *next; //指针域
}List;

void TailCreateList(List *L,char *data,int size)  //尾插法建立链表
{
    List *nl,*tail;//nl= new list,tmpl=temp list
	int i = 0;
    tail=L;
    while(NULL!=tail->next){
		tail=tail->next;
		i++;
	}
    //nl=(struct List*)malloc(sizeof(struct List));
    nl->data=(char*)malloc(size+1);
	nl->id = i;
    memcpy(nl->data,data,size);
	//nl->data[size+1]=0;
    nl->next=NULL;
    tail->next=nl;
}

void DisPlay(List *L)
{
    List *p=L->next;
    while(p!=NULL)
    {
        printf ("%d,%s \n",p->id,p->data);
        p=p->next;
    }
    printf("\n");
}

int main(int argc,char*argv[])
{
    char buf[MAX_STR_LEN];
    char *p;int size;
    List *head;
    head=(List *)malloc(sizeof(List));
    head->data=NULL;
    head->next=NULL;
	head->id = 0 ;
    gets(buf);
    while(0 != strlen(buf)){
        gets(buf);
        size = strlen(buf);
        TailCreateList(head,buf,size);
    }
    
    return 0;
}