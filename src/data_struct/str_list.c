#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#define MAX_STR_LEN 2000

typedef struct List
{
char *data;
struct List *next; //指针域
}List;

void TailCreatList(List *L,char *data,int size)  //尾插法建立链表
{
    List *nl,*tail;//nl= new list,tmpl=temp list
    tail=L;
    while(NULL!=tail->next){tail=tail->next;}
    //nl=(struct List*)malloc(sizeof(struct List));
    nl->data=(char*)malloc(size+1);
    memcpy(nl->data,data,size+1);
    nl->next=NULL;
    tail->next=nl;
}

void DisPlay(List *L)
{
    List *p=L->next;
    while(p!=NULL)
    {
        printf ("%s ",p->data);
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
    gets(buf);
    while(NULL!=buf){
        gets(buf);
        size = strlen(buf);
        TailCreateList(head,buf,size);
    }
    
    return 0;
}