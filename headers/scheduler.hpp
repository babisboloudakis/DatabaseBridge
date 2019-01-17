#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>

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
    // thread histogram task 
    void execute() {
        cout << "Hist job!" << endl;
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