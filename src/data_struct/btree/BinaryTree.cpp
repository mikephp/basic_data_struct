#include "stdafx.h"
#include"Tree.h"

void CreateBTree(BT * * bt,char *a){
	BT *p;
	BT *s[MS];
	int top = -1;
	int k;
	int i = 0;
	*bt = NULL;
	while (a[i]){
		switch(a[i]){
		case ' ':
			break;
		case '(':
			if(top == MS-1){printf("栈空间太小,需要增加MS的值！\n");exit(1);}
			top++;s[top]=p;k=1;break;
		case')':
			if(top == -1){printf("二叉树广义表字符串有错！\n");exit(1);}
			top--;break;
		case',':
			k=2;break;
		default:
			if((a[i]>='a'&&a[i]<='z')||(a[i]>='A'&&a[i]<='Z')){
			p = (BT*)malloc(sizeof(BT));
			p->data=a[i];p->left=p->right=NULL;
			if(*bt==NULL)*bt=p;
			else{if(k==1)s[top]->left=p;
				else s[top]->right=p;}
			}
			else{printf("二叉树广义表字符串错！\n");exit(1);}
		}
		i++;
	}
}

void InitBTree(BT**bt){
	*bt = NULL;
}
void Inorder(BT*bt){
	if(bt!=NULL){
		Inorder(bt->left);
		printf("%c\n",bt->data);
		Inorder(bt->right);
	}
}