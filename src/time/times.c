#include <stdio.h>
#include <sys/times.h>
 
int main(void)
{
    int i;
    clock_t start, end;
    struct tms tms_start, tms_end;
 
    start = times(&tms_start);
    end = times(&tms_end);
 
    printf("start clock time : %d\n", start);
    printf("end   clock time : %d\n", end);
 
    return(0);
}