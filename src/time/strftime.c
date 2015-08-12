#include <stdio.h>
#include <time.h>
 
int main(void)
{
    time_t now;
    struct tm *tm_now;
    char    datetime[200];
 
    time(&now);
    tm_now = localtime(&now);
    strftime(datetime, 200, "%x %X %n%Y-%m-%d %H:%M:%S %nzone: %Z\n", tm_now);
 
    printf("now datetime : %s\n", datetime);
 
    return(0);
}