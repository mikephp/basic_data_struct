/* ∫¡√Îº∂ —” ± */
#include <sys/time.h>
#include <stdio.h>

void mmsleep(int ms)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000; // 20 ms
	select(0, NULL, NULL, NULL, &delay);
}

