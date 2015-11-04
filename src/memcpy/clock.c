#include<stdio.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<sys/time.h>
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
	struct timeval timev1;
	struct timeval timev2;
	struct timeval timev3;
 	printf("%llu \n",CLOCKS_PER_SEC);
    start=clock();
	gettimeofday(&timev1,NULL);
    //dumpcopy();
	usleep(1000000);
	sleep(1);
    middle=clock();
	gettimeofday(&timev2,NULL);
    //smartcopy();
	usleep(1000000);
    end=clock();
	gettimeofday(&timev3,NULL);

    printf("%.7f\n",(middle-start)/(double)CLOCKS_PER_SEC);//Լ2.2s
    printf("%.7f\n",(end-middle)/(double)CLOCKS_PER_SEC);//Լ0.07s
	printf("%.7f\n",timev2.tv_sec-timev1.tv_sec+(timev2.tv_usec-timev1.tv_usec)/1000/1000);//Լ2.2s
	printf("%.7f\n",timev3.tv_sec-timev2.tv_sec+(timev3.tv_usec-timev2.tv_usec)/1000/1000);//Լ2.2s

    return 0;
}
