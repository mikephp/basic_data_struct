#include <stdio.h>
#include <time.h>
 
int main(void)
{
    time_t now;
    struct tm *tm_now;
    char *datetime;
 
    time(&now);
    tm_now = localtime(&now);
    datetime = asctime(tm_now);
 
    printf("now datetime: %s\n", datetime);
 
    return(0);
}