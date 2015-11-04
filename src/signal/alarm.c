#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void sigalrm_fn(int sig)
{
	static int i=0;
    printf("alarm! i=%d\n",i);
    alarm(2);
	i++;
    return;
}
int main(void)
{
    signal(SIGALRM, sigalrm_fn);
    alarm(1);
    while(1) pause();
}
