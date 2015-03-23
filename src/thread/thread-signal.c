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
    for(int i=0;i<100;i++)
    {
        printf("in pthread test \n");
        sleep(1);
    }
}

int main(void)
{
    printf("begin \n");
    pthread_create(&pid, NULL , test, NULL);
    sleep(3);
    if(pthread_kill(pid, 0)!= ESRCH)
    {
        printf("thread %d exists!\n", pid);
        pthread_kill(pid, SIGQUIT);
//        pthread_exit(NULL);//this won't work
        printf("after kill\n");
    }



    sleep(1);

    printf("exit in main\n");
}