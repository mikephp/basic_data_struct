#include <signal.h>  
#include <stdio.h>  
#include <unistd.h>  
  
void ouch(int sig)  
{  
   printf("I got signal %d\n", sig);  
   // (void) signal(SIGINT, SIG_DFL);  
  //(void) signal(SIGINT, ouch);  
 
}  
	
int main()  
{  
	(void) signal(SIGINT, ouch);  

	while(1)  
	{  
		printf("hello world...\n");  
		sleep(1);  
	}  
}