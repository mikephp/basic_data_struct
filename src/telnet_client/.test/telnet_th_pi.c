#include <stdio.h>  // printf
#include <stdlib.h> // exit
#include <unistd.h> // pipe
#include <string.h> // strlen
#include <pthread.h> // pthread_create

//using namespace std;

void *func(void * fd)
{
        //printf("write fd = %d\n", *(int*)fd);
        char str[128] ;
        //write( *(int*)fd, str, strlen(str) );
		while(1){
			memset(str,0,sizeof(str));
			read(*(int *)fd,str,sizeof(str));
			printf("%s\n",str);
			if(str[0]=='q'){
				printf("over\n");
				return 0;
			}
		}
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
        pthread_create(&tid, NULL, func, &fd[0]);
        //pthread_join(tid, NULL);

        sleep(1);
        write(fd[1],"ljy",3);       
        sleep(1);
		write(fd[1],"q",1);
		sleep(1);
        // read buf from child thread
        //read( fd[0], readbuf, sizeof(readbuf) );
        //printf("read buf = %s\n", readbuf);

        return 0;
}
