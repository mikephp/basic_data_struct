// BTree.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include"Tree.h"

void CreateBTree(BT * * bt,char *a);
void InitBTree(BT**bt);
void Inorder(BT*bt);
BT* CreateHuffman(int a[],int n);

int _tmain(int argc, _TCHAR* argv[])
{
	char *ch="A(B(C),D(E(F,G),H(,I)))";
	static int a[6]={3,9,5,12,6,15};int n=6;
	BT *p,*hufp;
	InitBTree(&p);
	CreateBTree(&p,ch);
	Inorder(p);
	hufp=CreateHuffman(a,n);
	Inorder(hufp);
	//printf("%d",sizeof(BT));
    system("PAUSE");	
	return 0;
}
