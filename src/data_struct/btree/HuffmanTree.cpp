#include "stdafx.h"
#include"Tree.h"
BT *CreateHuffman(int a[],int n){
	int i,j;
	BT**b,*q;
	b=(BT**)malloc(n*sizeof(BT*));
	for(i=0;i<n;i++){
		b[i]=(BT*)malloc(sizeof(BT));
		b[i]->data = a[i];b[i]->left=b[i]->right=NULL;
	}
		for(i=1;i<n;i++){
			int k1=-1,k2;
			for(j=0;j<n;j++){
				if(b[j]!=NULL&&k1==-1){k1=j;continue;}
				if(b[j]!=NULL){k2=j;break;}
			}
			for(j=k2;j<n;j++){
				if(b[j]!=NULL){
					if(b[j]->data<b[k1]->data){k2=k1;k1=j;}
					else if(b[j]->data<b[k2]->data)k2=j;
				}
			}
		q=(BT*)malloc(sizeof(BT));
		q->data = b[k1]->data+b[k2]->data;
		q->left = b[k1];q->right = b[k2];
		b[k1]=q;b[k2]=NULL;
	}
	free(b);
	return q;
}