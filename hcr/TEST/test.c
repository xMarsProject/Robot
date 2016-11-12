#include <stdio.h>
#include <time.h>
#include <sys/time.h>

void ihmSleep_ms(int milisec)
{
    struct timespec res;
    res.tv_sec = milisec/1000;
    res.tv_nsec = (milisec*1000000) % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &res, NULL);
}

int main()
{


   struct timeval stop, start;
gettimeofday(&start, NULL);

for (;;) 
  {
	ihmSleep_ms(1000); // attend 1 secondes
    gettimeofday(&stop, NULL);
    printf("top %lu\n", stop.tv_usec - start.tv_usec);
    if ((stop.tv_usec - start.tv_usec)>200) // toutes les 2 secondes
     {
		printf("toppppp %lu\n", stop.tv_usec - start.tv_usec);
		gettimeofday(&start, NULL);
    }
  }
     return 0;
}


//do stuff
