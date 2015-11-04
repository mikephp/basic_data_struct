#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
void go();
void stop1(),stop2();
pid_t p1,p2;
void main( )
{	
	int status = -1;
	int rtv;
	while((p1=fork( ) )==-1); /*�����ӽ���p1*/
	if (p1>0)
	{
	 while((p2=fork( ) )==-1); /*�����ӽ���p2*/
	  if(p2>0)
	 {	  
	  printf("Parent process %d\n", getpid());
	  signal(SIGINT,go); /*���յ��źţ�תgo*/
      pause();
	  sleep(2);
	  wait(NULL);
      wait(NULL);
	  printf("Parent process is killed!\n");
	  exit(0);
	 }
	  else
	 {	 
		printf("Process 2, pid %d\n", getpid());
		signal(SIGINT, SIG_IGN);//����Ĭ�ϵ� SIGINT�źŴ���
		signal(SIGUSR2, stop2);
		if(signal(SIGUSR2,stop2) == SIG_ERR) {
			printf("Can't catch SIGUR2");
		}
		pause();
		printf("Process 2 End\n");
	 }
	}
	else
	{	
		printf("Process 1, pid %d\n", getpid());
		signal(SIGINT, SIG_IGN);//����Ĭ�ϵ� SIGINT�źŴ���
		if(signal(SIGUSR1,stop1) == SIG_ERR) {
			printf("Can't catch SIGUR2");
		}
        pause();
		printf("Process 1 End\n");
	}
	printf("child exit status is %d\n", WEXITSTATUS(status));
}

void go()
{
	int rtv;
	printf("Func go\n");
	rtv = kill(p1,SIGUSR1); 	/*��p1�����ж��ź�16*/
	if(rtv) {
		printf("fail to send signal 16 to p1\n");
	} else {
	
		printf("Succed in sending signal 16 to p1\n");
	}
	rtv = kill(p2,SIGUSR2); 	/*��p2�����ж��ź�17*/
	if(rtv) {
		printf("fail to send signal 17 to p2\n");
	} else {
		printf("Succed in sending signal 17 to p2\n");
	}
}
void stop2()
{
	printf("Child process 2 is killed by parent!\n");
	exit(0);
}
void stop1()
{
	printf("Child process 1 is killed by parent!\n");
	exit(0);
}
