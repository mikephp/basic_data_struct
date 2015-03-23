#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

pthread_t pid;

void handle_quit(int signo)
{
    printf("in qq handle sig %d \n", signo);
    pthread_exit(NULL);
}

void* test(void *arg)
{
    signal(SIGQUIT,handle_quit );
    for(int i=0;i<10;i++)
    {
        printf("in pthread test \n");
        sleep(1);
    }
	pthread_exit(NULL);
}

int main(void)
{
	int ret;
    printf("begin \n");
    ret = pthread_create(&pid, NULL , test, NULL);
	printf("ret = %d,pid = %d\n",ret,pid);	
    sleep(2);

	if(pthread_kill(pid, 0)!= ESRCH)
    {
        printf("thread %d exists!\n", pid);
        pthread_kill(pid, SIGQUIT);
//        pthread_exit(NULL);//this won't work
        printf("after kill\n");
    }
	sleep(5);
	ret = pthread_cancel(pid);
	printf("pthread_cancel = %d\n",ret);
    printf("exit in main\n");
}
