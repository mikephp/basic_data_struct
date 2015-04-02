#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define LEN 256
int spl(char *buf,char *av[])
{	
	int ac=0;
	char *ch=NULL;
	ch = strtok(buf," ");
	while(NULL!=ch){
		av[ac]=ch;
		ac++;
		ch=strtok(NULL," ");
	}
	return ac;
}

int outf_1(int ac,char *av[],FILE* op)/*lost char*/
{   
    char buf[LEN];
	char *tmp;
	char *str;
    int i;
	int len;
	tmp = (char *)buf;
	str = av[0];
    for(i=0;i<ac;i++){
		str = strtrim(av[i]);
	   len=strlen(str);
      
       tmp++;
	  printf("%s:%d\n",str,len);
    }
	tmp =buf; 
	//printf("%s\n",tmp);
    //fwrite(buf,1,strlen(buf),op);
	return 0;
}

int main()
{
	int count =0 ;
	char buf[512];
	FILE *fp;
	fp = fopen("4.txt","r");
	fseek(fp,0,SEEK_SET);
	if(NULL==fp){
		printf("open t2.txt failed\n");
	}
	
	FILE *op;
	op = fopen("01.txt","at");
	fseek(op,0,SEEK_SET);
	if(NULL==op){
		printf("open 01.txt failed\n");
	}
	
	fgets(buf,256,fp);
	do{
		int ac,i;
		char *av[5];
		ac=spl(buf,av);
		outf_1(ac,av,op);
		for(i=0;i<ac;i++){
		//	printf("%s\n",av[i]);
		}
		memset(buf,0,LEN);
		fgets(buf,LEN,fp);
		//printf("count=%d\n",count++);
	}while(!feof(fp));
	fclose(fp);
	fclose(op);
    //system("PAUSE");	
	return 0;
}
