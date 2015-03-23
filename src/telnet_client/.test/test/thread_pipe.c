#include <stdio.h>  // printf
#include <stdlib.h> // exit
#include <unistd.h> // pipe
#include <string.h> // strlen
#include <pthread.h> // pthread_create

//using namespace std;

void *func(void * fd)
{
        printf("write fd = %d\n", *(int*)fd);
        char str[] = "hello everyone!";
        write( *(int*)fd, str, strlen(str) );
}

int main()
{
        int fd[2];
        char readbuf[1024];

        if(pipe(fd) < 0)
        {
                printf("pipe error!\n");
        }

        // create a new thread
        pthread_t tid = 0;
        pthread_create(&tid, NULL, func, &fd[1]);
        pthread_join(tid, NULL);

        sleep(3);

        // read buf from child thread
        read( fd[0], readbuf, sizeof(readbuf) );
        printf("read buf = %s\n", readbuf);

        return 0;
}
