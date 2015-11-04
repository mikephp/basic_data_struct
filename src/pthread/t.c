/*
 ============================================================================
 Name        : mutex_test.c
 Author      : ayanmw
 Version     :
 Copyright   : CopyRight ptopn.com
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
int x=0;
pthread_mutex_t mutex_x;//= PTHREAD_MUTEX_INITIALIZER;
void *client(int *data){
    pthread_mutex_lock(&mutex_x);
    fprintf(stdout,"%d:mutex_lock\n",data);
        x++;
        fprintf(stdout,"%d:mutex 正在修改x的值:%d\n",data,x);
        sleep(1);
    pthread_mutex_unlock(&mutex_x);
    fprintf(stdout,"%d:mutex_unlock\n",data);

}
int main(void) {
    int i=1;
    pthread_t threadInfo;
    pthread_attr_t attr;
	pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mutex_x,&attr);

    pthread_create(&threadInfo,NULL,client,(void *)1);
    pthread_create(&threadInfo,NULL,client,(void *)2);
    pthread_create(&threadInfo,NULL,client,(void *)3);

    pthread_join(threadInfo,NULL);
    //fprintf(stdout,"Hi");
    return EXIT_SUCCESS;
}