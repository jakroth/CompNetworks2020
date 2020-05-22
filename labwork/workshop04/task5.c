#include <stdio.h> 
#include <sys/time.h>   // for gettimeofday()
#include "ctap.h"       // for ctap tests


/* functions */
/* iterate through loops loops, return value of i */
long long iterate(long long loops){
    int i;
    for(i = 0; i < loops; i++){
        for(int j = 0; j < 1000; j++){}     // slow down the loops a bit, something was breaking at 10,000,000,000 loops
    }
    return i;
}

/* load start time in timeval struct, return success 1, or fail 0 */
int start(struct timeval *t){
    gettimeofday(t, NULL);
    if(t!=NULL){
        return 1;
    }
    return 0;
}

/* load stop time in timeval struct, return success 1, or fail 0 */
int stop(struct timeval *t){
    gettimeofday(t, NULL);
    if(t!=NULL){
        return 1;
    }
    return 0;
}

/* return difference in secs between two timevals */
float difference(struct timeval *t1, struct timeval *t2){
    float time;
    time = t2->tv_sec - t1->tv_sec;
    time += (float)(t2->tv_usec - t1->tv_usec)/1000000; // convert microseconds to seconds
    return time;
}

TESTS{   
    /* define appropriate structures */
    struct timeval startTime;
    struct timeval stopTime;
    float diffTime = 0;
    long long loops = 1000000;          // I increased loop by * 10 each time, then * 2 the last time. See CLI printouts.                       
    char timetaken[50];

    /* record time taken to iterate over loops loops */
    //note("Time to compute steps");                            // note() doesn't work because ctap.h is grabbing stdout.
    //ctap_diag(CTAP.priv_stdout, "Time to compute steps");     // ctap_diag() (which is what note() is defined as) only works if stdout is changed to CTAP's priv stdout. 
    
    diag("Time to compute with loops = %lld", loops);           // Replaced with diag(), which prints to stderr. 
    ok(start(&startTime) == 1, "get the start time");           // test whether load start time worked
    ok(iterate(loops) == loops, "iterate through the loop");    // test whether iteration worked
    ok(stop(&stopTime) == 1, "get the stop time");              // test whether load stop time worked

    /* compute elapsed time and convert to string */
    diffTime = difference(&startTime, &stopTime);
    sprintf(timetaken,"%f seconds", diffTime);
    diag("Compute time taken %s", timetaken);                   // note() replaced with diag()

    /* use the appropriate time unit for comparison */
    ok(diffTime < 60, "time was less than 60 seconds");         // test whether timetaken is less than 60 seconds

}
