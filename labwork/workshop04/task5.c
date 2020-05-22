#include <stdio.h> 
#include <sys/time.h>   // for gettimeofday()
#include "ctap.h"       // for ctap tests


/* functions */
/* iterate through loops loops, return value of i */
int iterate(int loops){
    int i;
    for(i = 0; i < loops; i++){
    }
    return i;
}

/* return microsecs in long */
int start(struct timeval *t){
    gettimeofday(t, NULL);
    if(t!=NULL){
        return 1;
    }
    return 0;
}

/* return microsecs in long */
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
    float time2;
    fprintf(stderr,"t1 secs %ld\n", t1->tv_sec);
    fprintf(stderr,"t2 secs %ld\n", t2->tv_sec);
    fprintf(stderr,"t1 microsecs %ld\n", t1->tv_usec);
    fprintf(stderr,"t2 microsecs %ld\n", t2->tv_usec);
    time = t2->tv_sec - t1->tv_sec;
    fprintf(stderr,"secs: %f\n", time);
    time2 = (t2->tv_usec - t1->tv_usec)/1000000; 
    fprintf(stderr,"microsecs: %f\n", time2);
    return time;
}

TESTS{   
    /* define appropriate structures */
        struct timeval startTime;
        struct timeval stopTime;
        float diffTime = 0;
        int loops = 10000000000;
        char timetaken[50];
        setbuf(stdout, NULL);

        /* record time taken to iterate over loops loops */
        note("Time to compute steps\n");                    // I couln't get note() to work. stdout is not working for some reason. 
        ctap_diag(stderr, "Time to compute steps");         // Couldn't make ctap_diag() work either. 
        diag("Time to compute steps");                      // Replaced with diag(), which prints to stderr. 
        ok(start(&startTime) == 1, "get the start time");
        ok(iterate(loops) == loops, "iterate through the loop");
        ok(stop(&stopTime) == 1, "get the stop time");
        printf("microsecs: ");
        fflush(stdout);   

        /* compute elapsed time and convert to string */
        diffTime = difference(&startTime, &stopTime);
        sprintf(timetaken,"%f seconds", diffTime);
        note("Compute time taken %s", timetaken);          // I couln't get note() to work.
        diag("Compute time taken %s", timetaken);          // Replaced with diag()

        /* use the appropriate time unit for comparison */
        ok(diffTime < 60, "time was less than 60 seconds");

    TODO("to implement"){ 
    }
}
