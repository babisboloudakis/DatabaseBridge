#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

// Abstract class, should be inherited to create a variety of jobs
class Job {
    
    public:
    // Pure virtual method, should be implemented by all sub-classes
    virtual void Execute() = 0;
    
}


class JobScheduler {

    private:

    // Jobs queue
    vector<Job *> queue;
    // Thread pool
    vector<int> threads;
    // Thread number
    int threadsCount;

    public:

    // Constructor and destructor methods
    JobScheduler( int count ) : threadsCount(count) ;

    inline ~JobScheduler() {
        // default
    }

    // Add job to the queue
    void add( Job * job );
    // Thread function for specific thread
    void run( int threadId );
    
}