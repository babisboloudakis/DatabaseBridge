#include "headers/scheduler.hpp"
#include <unistd.h>

// used to signal thread termination
int work = 1;
// used to break thread jobs
int current = 0;

using namespace std;

// Global variables
// JobScheduler scheduler;

void *threadFunction(void* arg) {
    // Function that every thread runs as soon
    // as it is created
    while ( work ) {

        Job* job = scheduler.getJob();
        job->execute();
        delete job; 
        pthread_mutex_lock(&scheduler.mutex);

        scheduler.jobCounter--;
        if ( scheduler.jobCounter == 0 ) {
            pthread_cond_signal(&scheduler.empty);
        }

        pthread_mutex_unlock(&scheduler.mutex);

    }
    // Exit as soon as jobs done
    pthread_exit(NULL);
}

int JobScheduler::init() {
    // Create defined number of threads
    // return their ids into the threads vector
    for(int i = 0; i < THREAD_NUMBER; i++) {
        pthread_create(&(this->threads[i]), NULL, threadFunction, NULL);
    }
    return 0;
}

int JobScheduler::destroy() {
    // Terminate all already executed threads
    work = 0;
    // Wait for them to exit
    for ( int i=0;i<THREAD_NUMBER;i++) {
        pthread_join(this->threads[i],NULL);
    }
    return 0;
}

void JobScheduler::barrierInit(int n) {
    pthread_mutex_lock(&this->mutex);
    this->jobCounter = n;
    pthread_mutex_unlock(&this->mutex);
}

void JobScheduler::barrier() {
    // NOT FUNCTIONAL YET
    pthread_mutex_lock(&this->mutex);
    while ( this->jobCounter > 0 ) {
        pthread_cond_wait(&this->empty, &this->mutex);
    }  
    pthread_mutex_unlock(&this->mutex);

}

void JobScheduler::schedule(Job* job) {
    // Lock mutex to safely insert job
    pthread_mutex_lock(&this->mutex);
    // Insert Job
    this->queue.push_back(job);
    pthread_cond_signal(&this->nonempty);
    // Unlock mutex so that the process can continue
    pthread_mutex_unlock(&this->mutex);
}

Job * JobScheduler::getJob() {
    // Lock Mutex to safely access queue.
    pthread_mutex_lock(&this->mutex);
    // Wait fora a task to arrive.
    while ( this->queue.size() <= 0 ) {

        pthread_cond_wait(&this->nonempty, &this->mutex);
        if ( work == 0 ) {
            // Exit in case thread gets termination signal
            pthread_mutex_unlock(&this->mutex);
            pthread_exit(0);
        }
        
    }
    // Get the highest priority Job.
    Job * job = this->queue[0];
    // Erase it from the vector.
    this->queue.erase(this->queue.begin()); 
    // Unlock mutex when done.
    pthread_mutex_unlock(&this->mutex);
    return job;
}



// int main ( void ) { 
    // Main function used to test our Job Scheduler.
    // scheduler.schedule(new HistJob(3) );
    // scheduler.schedule(new HistJob(12));
    // scheduler.schedule(new HistJob());
    // scheduler.schedule(new HistJob());
    // scheduler.schedule(new HistJob());
    // scheduler.schedule(new HistJob());/
    // scheduler.init();
    
    
    // scheduler.barrier();
    // scheduler.destroy();
    // return 0;
// }
