#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sequence_list.h"
#include "common.h"
int List_Init(SqList*List)
{
	if(!List){
		return -1;
	}
	memset(List,0,sizeof(SqList));
	return 0;
}

int List_Length(SqList*List)
{
	if(!List){
		return -1;
	}
	return List->length;
}

/*
*	return:	List subscript is the return value.
*		fail	:	-1,-2 and so on.
*		success	:	 0 - (maxsize-1).
*/
int List_LocateElem(SqList*List,EMTY*et)
{
	int i;
	if(!List){
		return -1;
	}
	for(i = 0;i < List->length;i++){
		#ifdef _COMPARE_SIMPLE_
		if( List->data[i] == *et )
			return i;
		#else
		if(memcmp(&List->data[i],et,sizeof(EMTY)))
			return i;
		#endif
	}
	return -2;
}

/*
*	*et:		store getted element value pointer
*	subscript: element subscript,from 0 to (maxsize - 1)
*	return:
*		fail	:	-1,-2
*		success	:	 0
*/
int List_GetElem(SqList*List,int subscript ,EMTY**et)
{
	if(!List|| subscript < 0|| !et){
		return -1;
	}
	if( subscript > (List->maxsize - 1)){
		return -2;
	}
	*et = &List->data[subscript];
	return 0;
}

int List_Insert	(SqList*List,int subscript,EMTY*et)
{
	int i;
	if(!List || subscript < 0){
		return -1;
	}
	if( subscript > (List->length)){
		return -2;
	}
	for(i = List->length; i > subscript; i++){
		List->data[i] = List->data[i - 1];
		#ifdef _COMPARE_SIMPLE_
			List->data[i] = List->data[i - 1];
		#else
			memcpy(&List->data[i],&List->data[i - 1],sizeof(EMTY));
		#endif
	}
	#ifdef _COMPARE_SIMPLE_
		List->data[subscript]=*et;
	#else
		memcpy(&List->data[subscript],et,sizeof(EMTY));
	#endif
	List->length++;

	return 0;
}

int List_Delete	(SqList*List,int subscript,EMTY*et)
{
	if(!List){
		return -1;
	}
	return 0;
}

int List_Print	(SqList*List)
{
	int i;
	char buf[20];
	if(!List){
		return -1;
	}
	for(i = 0;i < List->length;i++){
		sprintf(buf,"LIST %d",i);
		printc(buf,sizeof(EMTY),&List->data[i]);
	}
	return 0;
}

int List_IsEmpty(SqList*List)
{
	if(!List){
		return -1;
	}
	return List->length;
}

int List_IsFull	(SqList*List)
{
	if(!List){
		return -1;
	}
	if(List->length == List->maxsize)
		return 1;
	else
		return 0;
}

int List_Destroy(SqList*List)
{
	if(!List){
		return -1;
	}
	memset(List,0,sizeof(SqList));
	//free(List);
	return 0;
}

int List_Reverse(SqList*List)
{
	if(!List){
		return -1;
	}
	return 0;
}
