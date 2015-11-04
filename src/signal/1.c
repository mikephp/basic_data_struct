#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void go();
void stop1(),stop2();

int p1,p2;

main( )
{	
	while((p1=fork( ) )==-1); /*�����ӽ���p1*/
	if (p1>0)
	{
	 while((p2=fork( ) )==-1); /*�����ӽ���p2*/
	  if(p2>0)
	 {	  
          printf("This is parent %d.\n", getpid()); 
	  signal(SIGINT,go); /*���յ��źţ�תgo*/
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
	  signal(17,stop2); /*���յ����ж��ź�17��תstop2*/
          pause();
	 }
	}
	else if(p1 == 0)
	{	
         printf("This is child_1 %d.\n", getpid()); 
	 signal(16,stop1); /*���յ����ж��ź�16��תstop1*/
         pause();
	}
}

void go()
{
	kill(p1,16); 	/*��p1�����ж��ź�16*/
	kill(p2,17); 	/*��p2�����ж��ź�17*/
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