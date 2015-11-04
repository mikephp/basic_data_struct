#include<stdio.h>
int main()
{
	FILE* binaryp;
	int i;
	binaryp = fopen("nums.bin","wb");
	for(i=2;i<500;i+=2){
		fwrite(&i,sizeof(int),1,binaryp);
	}
	fclose(binaryp);
	return 0;
}
