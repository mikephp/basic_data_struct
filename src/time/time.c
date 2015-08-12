#include <stdio.h>
#include <time.h>
 
int main(void)
{
    time_t now;
    time(&now);
    printf("now time is %lld\n", now);
 
    return(0);
}