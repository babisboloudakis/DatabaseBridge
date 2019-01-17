#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

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
    void execute();
};

// Definition of thread function
void* threadFunction();

class JobScheduler {

    private:

    // Jobs queue
    vector<Job *> queue;
    // Thread pool
    pthread_t threads[THREAD_NUMBER];


    public:
    // Constructor and destructor methods
    inline JobScheduler() {
        // default
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

}