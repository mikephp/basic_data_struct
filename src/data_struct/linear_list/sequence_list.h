#ifndef _SQUENCE_LIST_H
#define _SQUENCE_LIST_H

//#undef _COMPARE_SIMPLE_

#ifndef SEQUENCE_LIST_MAX_SIZE
#define SEQUENCE_LIST_MAX_SIZE 100
#endif
// element type
typedef int EMTY;

typedef struct{
	EMTY data[SEQUENCE_LIST_MAX_SIZE];
	int length;
	int maxsize;
}SqList;

int List_Init(SqList*);
int List_Length(SqList*);

/*
*	fail	:	return -1
*	success	:	return  0 - (maxsize-1)
*/
int List_LocateElem(SqList*,EMTY*);

int List_GetElem(SqList*,int,EMTY**);
int List_Insert	(SqList*,int,EMTY*);
int List_Delete	(SqList*,int,EMTY*);
int List_Print	(SqList*);
int List_IsEmpty(SqList*);
int List_IsFull	(SqList*);
int List_Destroy(SqList*);
int List_Reverse(SqList*);

#endif
