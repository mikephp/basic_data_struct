/*
 *
 *	Author:	mikephp
 *	Created Time:	2015/12/1 22:28
 *	Function:	judge a linked list is or not a loop linked list
 * 
 */
#include <stdio.h>
#include <stdlib.h>

#define LEN 10

struct node{  
    char val;  
    struct node *next;  
};  

typedef struct node* node_t;
typedef struct node  node_v;

//method 1
int has_loop(struct node *head);
//method 2
int has_loop2(node_t head);

int main()
{
    node_t arr[LEN];
    node_v nd[LEN];
	int i = 0;
	for (i = 0; i < LEN; i++){
		arr[i] = &nd[i];
	}
	
    for(i = 1; i < LEN; i++)
    {
        arr[i - 1]->next = arr[i];
    }
	arr[LEN - 1]->next = NULL;

    //you can add a loop here to test
	//arr[LEN - 2]->next = arr[LEN -3];
    if (has_loop(arr[0]))
        printf("method1: has loop.\n");
    else
        printf("method1: has no loop.\n");

    if (has_loop2(arr[0]))
        printf("method2: has loop.\n");
    else
        printf("method2: has no loop.\n");

    return 0;
}

//if two pointer are equal, but they don't have the same steps, then has a loop
int has_loop(node_t head)  
{  
    node_t cur1 = head;  
    int pos1 = 0;  
    while(cur1){  
        node_t cur2 = head;  
        int pos2 = 0;  
        pos1 ++;  
        while(cur2){  
            pos2 ++;  
            if(cur2 == cur1){  
                if(pos1 == pos2)  
                    break;  
                else  
                    return 1;
            }  
            cur2 = cur2->next;  
        }  
        cur1 = cur1->next;  
    }  
    return 0;  
} 

//using step1 and step2 here 
//if exists a loop, then the pointer which use step2 will catch up with the pointer which uses step1
int has_loop2(node_t head)
{
    node_t p = head;
    node_t q = head;
    while (p != NULL && q != NULL)
    {
		p = p->next;
        q = q->next;
        if (q != NULL)
            q = q->next;
        if (p != NULL && p == q)
            return 1;
    }
    return 0;
}
