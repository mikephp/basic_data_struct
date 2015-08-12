#include <stdio.h>
#include <time.h>
 
int main(void)
{
    time_t now, new_time;
    struct tm *tm_now;
 
    time(&now);
    printf("now time is %ld\n", now);
 
    tm_now = localtime(&now);
 
    new_time = mktime(tm_now);
    printf("new time is %ld\n", new_time);
 
    return(0);
}