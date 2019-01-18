#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "headers/join.hpp"

using namespace std;

#define THREAD_NUMBER 3

// Abstract class, should be inherited to create a variety of jobs
class Job {
    
    public:
    // Pure virtual method, should be implemented by all sub-classes
    virtual void execute() = 0;
    
};

class HistJob : public Job {

    public:
    // Starting column
    package * data;
    // Start index
    int from;
    // End index
    int to;
    // n for hash function.
    int n;
    // Histogram 
    unsigned int * hist;


    // Job constructor.
    HistJob(package* data, int from, int to, int n, unsigned int * hist) : data(data), from(from), to(to), n(n), hist(hist) {}

    // thread histogram task 
    void execute() {
        for (int i = from; i < to; i++) {
            uint64_t payload = data[i].payload;
            uint64_t bucket = HashFunction1(payload, n);
            // increase appropriate hist counter
            hist[bucket]++;
        }
    }
};

class MergeJob : public Job {
    public:
    void execute() {
        cout << "MERGE JOB" << endl;
    }
};

// Definition of thread function
void* threadFunction();

class JobScheduler {

    private:

    // Jobs queue
    vector<Job*> queue;
    // Thread pool
    pthread_t threads[THREAD_NUMBER];
    // Mutexes etc.
    pthread_mutex_t mutex;
    pthread_cond_t nonempty;

  public:
    // Constructor and destructor methods
    inline JobScheduler() {
        // default
        this->mutex = PTHREAD_MUTEX_INITIALIZER;
        this->nonempty = PTHREAD_COND_INITIALIZER;
    }

    inline ~JobScheduler() {
        // default
    }

    // Initializes the JobScheduler with the number of open threads.
    // Returns true if everything done right false else.
    int init();
    // Free all resources that the are allocated by JobScheduler
    // Returns true if everything done right false else.
    int destroy();
    // Wait for all threads to finish their job
    void barrier();

    // Job methods
    // Schedule a job for execution by threads.
    void schedule(Job* job);
    // Get a job from the queue
    Job * getJob();

};