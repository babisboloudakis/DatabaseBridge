#include "headers/scheduler.hpp"
#include <unistd.h>

// used to signal thread termination
int work = 1;

using namespace std;


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
    pthread_cond_broadcast(&this->nonempty);
    // Wait for them to exit
    for ( int i=0;i<THREAD_NUMBER;i++) {
        pthread_join(this->threads[i],NULL);
    }
    return 0;
}

void JobScheduler::barrierInit(int n) {
    // Initialize the number of jobs to be executed with n.
    pthread_mutex_lock(&this->mutex);
    this->jobCounter = n;
    pthread_mutex_unlock(&this->mutex);
}

void JobScheduler::barrier() {
    // Suspend execution until all threads complete their tasks.
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


// Thread jobs

void HistJob::execute()
{
    for (int i = from; i < to; i++)
    {
        uint64_t payload = data[i].payload;
        uint64_t bucket = HashFunction1(payload, this->n);
        // increase appropriate hist counter
        hist[bucket]++;
    }
}

void PartJob::execute()
{
    for (int i = from; i < to; i++)
    {
        int bucket = HashFunction1(data[i].payload, this->n);
        datat[psum[bucket]++] = data[i];
    }
}

void JoinJob::execute()
{

    uint64_t last;
    uint64_t *chain;

    //select smallest bucket
    if (HistogramR[bucketIndex] <= HistogramS[bucketIndex])
    {
        chain = new uint64_t[HistogramR[bucketIndex]];
        for (int i = 0; i < HistogramR[bucketIndex]; i++)
        {
            uint64_t bucket = HashFunction2(bucketR[i].payload); // bucket for that payload
            last = bucketArray[bucket];
            bucketArray[bucket] = i + 1;
            chain[i] = last;
        }
        //go to other bucket
        for (int i = 0; i < HistogramS[bucketIndex]; i++)
        {
            uint64_t bucket = HashFunction2(bucketS[i].payload); // bucket for that payload
            if ((last = bucketArray[bucket]) != 0)
            { //if there is a hash
                if (bucketR[last - 1].payload == bucketS[i].payload)
                { //if there is a match
                    //add to result
                    int ind1 = bucketR[last - 1].index;
                    int ind2 = bucketS[i].index;
                    for (int k = 0; k < joinedMid->rels->size(); k++)
                    {
                        if (k < results1.rels->size())
                        {
                            pthread_mutex_lock(&scheduler.midResultsMutex);
                            joinedMid->res->at(k).rowIds->push_back(results1.res->at(k).rowIds->at(ind1));
                            pthread_mutex_unlock(&scheduler.midResultsMutex);
                        }
                        else
                        {
                            pthread_mutex_lock(&scheduler.midResultsMutex);
                            joinedMid->res->at(k).rowIds->push_back(results2.res->at(k - results1.rels->size()).rowIds->at(ind2));
                            pthread_mutex_unlock(&scheduler.midResultsMutex);
                        }
                    }
                }
                //go to next
                while (chain[last - 1] != 0)
                {
                    last = chain[last - 1];
                    if (bucketR[last - 1].payload == bucketS[i].payload)
                    {
                        //add to result
                        // Result * result = new Result;
                        // result->key1 = bucketR[last-1].key;
                        // result->key2 = bucketS[i].key;
                        int ind1 = bucketR[last - 1].index;
                        int ind2 = bucketS[i].index;
                        for (int k = 0; k < joinedMid->rels->size(); k++)
                        {
                            if (k < results1.rels->size())
                            {
                                pthread_mutex_lock(&scheduler.midResultsMutex);
                                joinedMid->res->at(k).rowIds->push_back(results1.res->at(k).rowIds->at(ind1));
                                pthread_mutex_unlock(&scheduler.midResultsMutex);
                            }
                            else
                            {
                                pthread_mutex_lock(&scheduler.midResultsMutex);
                                joinedMid->res->at(k).rowIds->push_back(results2.res->at(k - results1.rels->size()).rowIds->at(ind2));
                                pthread_mutex_unlock(&scheduler.midResultsMutex);
                            }
                        }
                        // delete result;
                    }
                }
            }
        }
    }
    else
    {
        chain = new uint64_t[HistogramS[bucketIndex]];
        for (int i = 0; i < HistogramS[bucketIndex]; i++)
        {
            uint64_t bucket = HashFunction2(bucketS[i].payload); // bucket for that payload
            uint64_t last = bucketArray[bucket];
            bucketArray[bucket] = i + 1;
            chain[i] = last;
        }
        //go to other bucket
        for (int i = 0; i < HistogramR[bucketIndex]; i++)
        {
            uint64_t bucket = HashFunction2(bucketR[i].payload); // bucket for that payload
            if ((last = bucketArray[bucket]) != 0)
            { //if there is a hash
                if (bucketS[last - 1].payload == bucketR[i].payload)
                { //if there is a match
                    //add to result
                    int ind1 = bucketR[i].index;
                    int ind2 = bucketS[last - 1].index;
                    for (int k = 0; k < joinedMid->rels->size(); k++)
                    {
                        if (k < results1.rels->size())
                        {
                            pthread_mutex_lock(&scheduler.midResultsMutex);
                            joinedMid->res->at(k).rowIds->push_back(results1.res->at(k).rowIds->at(ind1));
                            pthread_mutex_unlock(&scheduler.midResultsMutex);
                        }
                        else
                        {
                            pthread_mutex_lock(&scheduler.midResultsMutex);
                            joinedMid->res->at(k).rowIds->push_back(results2.res->at(k - results1.rels->size()).rowIds->at(ind2));
                            pthread_mutex_unlock(&scheduler.midResultsMutex);
                        }
                    }
                }
                //go to next
                while (chain[last - 1] != 0)
                {
                    last = chain[last - 1];
                    if (bucketS[last - 1].payload == bucketR[i].payload)
                    {
                        //add to result
                        int ind1 = bucketR[i].index;
                        int ind2 = bucketS[last - 1].index;
                        for (int k = 0; k < joinedMid->rels->size(); k++)
                        {
                            if (k < results1.rels->size())
                            {
                                pthread_mutex_lock(&scheduler.midResultsMutex);
                                joinedMid->res->at(k).rowIds->push_back(results1.res->at(k).rowIds->at(ind1));
                                pthread_mutex_unlock(&scheduler.midResultsMutex);
                            }
                            else
                            {
                                pthread_mutex_lock(&scheduler.midResultsMutex);
                                joinedMid->res->at(k).rowIds->push_back(results2.res->at(k - results1.rels->size()).rowIds->at(ind2));
                                pthread_mutex_unlock(&scheduler.midResultsMutex);
                            }
                        }
                    }
                }
            }
        }
    }

    // Delete allocated data structures.
    delete chain;
    delete bucketArray;
    delete bucketS;
    delete bucketR;
}