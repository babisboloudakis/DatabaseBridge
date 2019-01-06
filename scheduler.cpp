#include "headers/scheduler.hpp"

// used to signal thread termination
int done = 0;
// used to break thread jobs
int current = 0;



void* threadFunction() {
    // Function that every thread runs as soon
    // as it is created
    while ( !done ) {

        //get job
        //execute it

    }
    // Exit as soon as jobs done
    pthread_exit(NULL);
}

int JobSceduler::init() {
    // Create defined number of threads
    // return their ids into the threads vector
    for(int i = 0; i < THREAD_NUMBER; i++) {
        pthread_create( &(self->threads[i]), NULL, threadFunction, NULL );
    }
    return 0;
}

int JobScheduler::destroy() {
    // Terminate all already executed threads
    done = 1;
    // Wait for them to exit
    for ( int i=0;i<THREAD_NUMBER;i++) {
        pthread_join(self->threads[i],NULL);
    }
    return 0;
}

