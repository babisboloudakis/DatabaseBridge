#include "headers/scheduler.hpp"

// Returns the value of the n less significant bits of a payload
int HashFunction1( uint64_t payload, int n ) {
    return payload & ((1 << n) - 1);
}

// We modulo the payload with a prime number
// so that we get better divisions
int HashFunction2( uint64_t payload ) {
    return payload % PRIME; 
}

JobScheduler scheduler;

// Thread version
MidResult *Join::join(MidResult &results1, MidResult &results2, JoinInfo &join, FileArray &fileArray)
{
    scheduler.init();

    int index1, index2;
    for (int i = 0; i < results1.rels->size(); i++)
    {
        if (results1.rels->at(i) == join.rel1)
            index1 = i;
    }
    for (int i = 0; i < results2.rels->size(); i++)
    {
        if (results2.rels->at(i) == join.rel2)
            index2 = i;
    }

    // New vector with results after joining
    MidResult *joinedMid = new MidResult;
    joinedMid->rels = new vector<int>;
    joinedMid->res = new vector<RelationResults>;

    // Initialize relations in new mid result
    for (int i = 0; i < results1.rels->size(); i++)
    {
        joinedMid->rels->push_back(results1.rels->at(i));
    }
    for (int i = 0; i < results2.rels->size(); i++)
    {
        joinedMid->rels->push_back(results2.rels->at(i));
    }
    // Initialize relation into new mid result
    for (int i = 0; i < joinedMid->rels->size(); i++)
    {
        RelationResults relation;
        relation.relPos = joinedMid->rels->at(i);
        relation.rowIds = new vector<uint64_t>;
        joinedMid->res->push_back(relation);
    }

    // Use the two vectors to create relations structs
    relation relR;
    relR.num_tuples = results1.res->at(index1).rowIds->size();
    relR.tuples = new package[relR.num_tuples];
    vector<uint64_t> *values1 = fileArray.findColByRowIds(*(results1.res->at(index1).rowIds), join.col1, join.rel1);
    for (int i = 0; i < relR.num_tuples; i++)
    {
        relR.tuples[i].key = results1.res->at(index1).rowIds->at(i);
        relR.tuples[i].index = i;
        relR.tuples[i].payload = values1->at(i);
    }

    // Use the two vector to create relations structs
    relation relS;
    relS.num_tuples = results2.res->at(index2).rowIds->size();
    relS.tuples = new package[relS.num_tuples];
    vector<uint64_t> *values2 = fileArray.findColByRowIds(*(results2.res->at(index2).rowIds), join.col2, join.rel2);
    for (int i = 0; i < relS.num_tuples; i++)
    {
        relS.tuples[i].key = results2.res->at(index2).rowIds->at(i);
        relS.tuples[i].index = i;
        relS.tuples[i].payload = values2->at(i);
    }

    // -------------------------------------------
    // |  FIRST PART - HISTOGRAMS / PARTITIONING |
    // -------------------------------------------
    // Get the size of each relation
    int Rn = relR.num_tuples;
    int Sn = relS.num_tuples;
    // Get the relationship arrays
    package *R = relR.tuples;
    package *S = relS.tuples;
    // Also create the two new arrays
    package *Rt = new package[Rn];
    package *St = new package[Sn];
    // Generate bucket number
    int n = 3;
    int temp = n;
    int buckets = 1;
    // Calculate 2^n
    while (temp)
    {
        buckets *= 2;
        temp--;
    }

    // Create Histogram for buckets
    unsigned int **histR = new unsigned int* [THREAD_NUMBER];
    for ( int i = 0; i < THREAD_NUMBER; i++ ) {
        histR[i] = new unsigned int[buckets];
    }
    unsigned int **histS = new unsigned int* [THREAD_NUMBER];
    for (int i = 0; i < THREAD_NUMBER; i++) {
        histS[i] = new unsigned int[buckets];
    }

    // Initialize Histogram
    for (int i = 0; i < THREAD_NUMBER; i++) {
        for ( int j = 0; j < buckets; j++ ) {
            histR[i][j] = 0;
            histS[i][j] = 0;
        }
    }

    // Create thread Histogram Jobs.
    int current = 0;
    int step = Rn / THREAD_NUMBER;
    // Initialize barrier with appropriate number of jobs.
    scheduler.barrierInit(THREAD_NUMBER*2);
    for ( int i = 0; i < THREAD_NUMBER; i++ )
    {
        if ( i == THREAD_NUMBER - 1 ) {
            scheduler.schedule( new HistJob(R, current, Rn, n, histR[i] ) );
        } else {
            scheduler.schedule( new HistJob(R, current, current+step, n, histR[i] ) );
            current += step;
        }
    }
    // Same for second relation
    current = 0;
    step = Sn / THREAD_NUMBER;
    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        if (i == THREAD_NUMBER - 1)
        {
            scheduler.schedule(new HistJob(S, current, Sn, n, histS[i]));
        }
        else
        {
            scheduler.schedule(new HistJob(S, current, current + step, n, histS[i]));
            current += step;
        }
    }

    // Wait for them to finish
    scheduler.barrier();

    // Create the sum of histograms
    unsigned int *HistogramR = new unsigned int[buckets];
    unsigned int *HistogramS = new unsigned int[buckets];
    for ( int i = 0; i < buckets; i++ ) {
        HistogramR[i] = 0;
        HistogramS[i] = 0;
        for ( int j = 0; j < THREAD_NUMBER; j++ ) {
            HistogramR[i] += histR[j][i];
            HistogramS[i] += histS[j][i];
        }
    }
    // Create Psum Array using Histogram sum
    unsigned int *psumR = new unsigned int[buckets];
    unsigned int *psumS = new unsigned int[buckets];
    // Generate Psum data
    for (int i = 0; i < buckets; i++)
    {
        if (i == 0)
        {
            psumR[0] = 0;
            psumS[0] = 0;
            continue;
        }
        psumR[i] = psumR[i - 1] + HistogramR[i - 1];
        psumS[i] = psumS[i - 1] + HistogramS[i - 1];
    }
    // Create thread atomic Psums
    unsigned int **psR = new unsigned int*[THREAD_NUMBER];
    unsigned int **psS = new unsigned int *[THREAD_NUMBER];
    for ( int i = 0; i < THREAD_NUMBER; i++ ) {
        psR[i] = new unsigned int[buckets];
        psS[i] = new unsigned int[buckets];
    }
    for ( int i = 0; i < THREAD_NUMBER; i++ ) {
        for ( int j = 0; j < buckets; j++ ) {
            if ( i == 0 ) {
                psR[0][j] = psumR[j];
                psS[0][j] = psumS[j];
            } else {
                int tempr = 0;
                int temps = 0;
                for ( int k = 0; k < i; k++ ) {
                    tempr += histR[k][j];
                    temps += histS[k][j];
                }
                psR[i][j] = psumR[j]+tempr;
                psS[i][j] = psumS[j]+temps;
            }
        }
    }
    // Create thread Partion Jobs.
    current = 0;
    step = Rn / THREAD_NUMBER;
    // Initialize barrier with appropriate number of tasks.
    scheduler.barrierInit(THREAD_NUMBER * 2);
    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        if (i == THREAD_NUMBER - 1)
        {
            scheduler.schedule(new PartJob(R, Rt, psR[i], current, Rn, n ));
        }
        else
        {
            scheduler.schedule(new PartJob(R, Rt, psR[i], current, current + step, n));
            current += step;
        }
    }
    // Same for second relation
    current = 0;
    step = Sn / THREAD_NUMBER;
    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        if (i == THREAD_NUMBER - 1)
        {
            scheduler.schedule(new PartJob(S, St, psS[i], current, Sn, n));
        }
        else
        {
            scheduler.schedule(new PartJob(S, St, psS[i], current, current + step, n));
            current += step;
        }
    }
    // Wait for partitioning to finish before moving on.
    scheduler.barrier();

    // -----------------------
    // |  SECOND PART - JOIN |
    // -----------------------
    //initialiaze index and offsets
    int offsetR = 0;
    int offsetS = 0;

    scheduler.barrierInit(buckets);
    //for loop
    for (int bucketIndex = 0; bucketIndex < buckets; bucketIndex++)
    {
        //initialize bucket arrays
        package *bucketR = new package[HistogramR[bucketIndex]];
        package *bucketS = new package[HistogramS[bucketIndex]];

        for (int i = 0; i < HistogramR[bucketIndex]; i++)
        {
            bucketR[i] = Rt[i + offsetR];
        }
        for (int i = 0; i < HistogramS[bucketIndex]; i++)
        {
            bucketS[i] = St[i + offsetS];
        }
        //initialize bucketArray
        uint64_t *bucketArray = new uint64_t[PRIME];
        for (int i = 0; i < PRIME; i++)
        {
            bucketArray[i] = 0;
        }

        // THREADS HERE
        scheduler.schedule( new JoinJob(HistogramR, HistogramS, bucketR, bucketS, bucketArray, joinedMid, results1, results2, bucketIndex, offsetR, offsetS ) );
        offsetR = offsetR + HistogramR[bucketIndex];
        offsetS = offsetS + HistogramS[bucketIndex];
        
    }

    scheduler.barrier();
    
    // ----------------
    // |  THIRD PART  |
    // ----------------

    // Delete of histograms and psums
    delete HistogramR;
    delete HistogramS;
    for ( int i = 0; i < THREAD_NUMBER; i++ ) {
        delete histR[i];
        delete histS[i];
    }
    delete histR;
    delete histS;

    delete psumR;
    delete psumS;
    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        delete psR[i];
        delete psS[i];
    }
    delete psR;
    delete psS;
    // Delete of final arrays
    delete Rt;
    delete St;

    // Delete of previous rowId vectors
    delete results1.rels;
    delete results2.rels;
    for (int i = 0; i < results1.res->size(); i++)
    {
        delete results1.res->at(i).rowIds;
    }
    delete results1.res;
    for (int i = 0; i < results2.res->size(); i++)
    {
        delete results2.res->at(i).rowIds;
    }
    delete results2.res;

    delete values1;
    delete values2;
    // Replace with our new vectors
    return joinedMid;
}




void Join::join(MidResult & results, JoinInfo & join, FileArray & fileArray ){
    int index1;
    int index2;
    vector<uint64_t> * val1, *val2;
    if((join.rel1 == join.rel2) && (join.col1 == join.col2)){
        return;
    }
    vector<RelationResults> * newRes = new vector<RelationResults>;
    for(int i=0; i < results.rels->size(); i++){
        RelationResults temp;
        temp.relPos = results.rels->at(i);
        temp.rowIds = new vector<uint64_t>;
        newRes->push_back(temp);
    }
    for (int i=0; i<results.rels->size(); i++){
        if ( results.rels->at(i) == join.rel1){
            index1 = i;
        }
        if (results.rels->at(i) == join.rel2){
            index2 = i;
        }
    }

    val1 = fileArray.findColByRowIds(*(results.res->at(index1).rowIds), join.col1, join.rel1);
    val2 = fileArray.findColByRowIds(*(results.res->at(index2).rowIds), join.col2, join.rel2);
    
    for (int i=0; i < results.res->at(index1).rowIds->size(); i++){
        if ((*val1)[i] == (*val2)[i]){
            for (int j=0; j<results.rels->size(); j++){
                newRes->at(j).rowIds->push_back(results.res->at(j).rowIds->at(i));
            }
        }
    }

    for (int j=0; j<results.rels->size(); j++){
        delete (results.res->at(j).rowIds);
    }
    delete (results.res);
    results.res = newRes;
    
    return;
}

