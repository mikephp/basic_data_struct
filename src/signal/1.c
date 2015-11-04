#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void go();
void stop1(),stop2();

int p1,p2;

main( )
{	
	while((p1=fork( ) )==-1); /*创建子进程p1*/
	if (p1>0)
	{
	 while((p2=fork( ) )==-1); /*创建子进程p2*/
	  if(p2>0)
	 {	  
          printf("This is parent %d.\n", getpid()); 
	  signal(SIGINT,go); /*接收到信号，转go*/
          pause();

          sleep(2);               // wait for the operation of child

	  wait(0);
          wait(0);
	  printf("Parent process is killed!\n");
	  exit(0);
	 }
	  else if(p2 == 0)
	 {	
          printf("This is child_2 %d.\n", getpid()); 
	  signal(17,stop2); /*接收到软中断信号17，转stop2*/
          pause();
	 }
	}
	else if(p1 == 0)
	{	
         printf("This is child_1 %d.\n", getpid()); 
	 signal(16,stop1); /*接收到软中断信号16，转stop1*/
         pause();
	}
}

void go()
{
	kill(p1,16); 	/*向p1发软中断信号16*/
	kill(p2,17); 	/*向p2发软中断信号17*/
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