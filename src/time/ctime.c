#include <stdio.h>
#include <time.h>
 
int main(void)
{
    time_t now;
    char *datetime;
 
    time(&now);
    datetime = ctime(&now);
 
    printf("now datetime: %s\n", datetime);
 
    return(0);
}