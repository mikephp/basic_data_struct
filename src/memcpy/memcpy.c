#include<stdio.h>
#include<string.h>
#include<time.h>
char dest[65536],source[65536];
 
void dumpcopy()
{
    for(int i=0;i<10000;i++)
    for(int i=0;i<65536;++i){
        dest[i]=source[i];
    }
}
void smartcopy(){
    for(int i=0;i<10000;i++)
    memcpy(dest,source,65536);
}
 
int main()
{
    clock_t start,middle,end;
 	printf("%llu \n",CLOCKS_PER_SEC);
    start=clock();
    dumpcopy();
    middle=clock();
    smartcopy();
    end=clock();
 
    printf("%.7f\n",(middle-start)/(double)CLOCKS_PER_SEC);//Լ2.2s
    printf("%.7f\n",(end-middle)/(double)CLOCKS_PER_SEC);//Լ0.07s
    return 0;
}