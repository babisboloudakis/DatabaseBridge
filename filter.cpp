#include <iostream>
#include "parser.hpp"
#include <vector>
#include <cstdint>
#include <cstring>

#define BUFFERSIZE 1048576 // 1024 * 1024 = 1 MB
#define PRIME 7 // prime number used in HashFunction2

using namespace std;

void filterResults( RelationResults & results, FilterInfo filter, FileArray & fileArray ) {

    // New vector with results after filtering
    vector<uint64_t> * filteredResults = new vector<uint64_t>;

    // Get the values to be filtered throught the mid results
    vector<uint64_t> * values = fileArray.findColByRowIds( *(results.rowIds), filter.col, filter.rel );

    // Loop throught results and apply filter
    for ( int index = 0; index < values->size(); index++ ) {

        // Execute selected filter operation
        switch( filter.op ) {
            case FilterInfo::FilterOperation::LESS:
                if ( (*values)[index] < filter.constant ) { 
                    filteredResults->push_back( (*results.rowIds)[index] );
                }
                break;
            case FilterInfo::FilterOperation::GREATER:
                if ( (*values)[index] > filter.constant ) {
                    filteredResults->push_back( (*results.rowIds)[index] );
                }
                break;
            case FilterInfo::FilterOperation::EQUAL:
                if ( (*values)[index] == filter.constant ) {
                    filteredResults->push_back( (*results.rowIds)[index] );
                }
                break;
        }

    }

    // delete previous unnecessary vector
    delete results.rowIds;
    results.rowIds = filteredResults;
    // Delete values allocated by findColByRowIds
    delete values;

}


/** Type definition for a tuple */
typedef struct package {
    uint64_t key;
    uint64_t payload;
} package;

/**
* Type definition for a relation.
* It consists of an array of tuples and a size of the relation.
*/
typedef struct relation {
    package * tuples;
    uint64_t num_tuples;
} relation;

/**
* Type definition for a result.
* It consists of a tuple of 2 keys for each rowId.
*/
typedef struct Result {
    uint64_t key1;
    uint64_t key2;
} Result; 

// Returns the value of the n less significant bits of a payload
int HashFunction1( uint64_t payload, int n ) {
    return payload & ((1 << n) - 1);
}

// We modulo the payload with a prime number
// so that we get better divisions
int HashFunction2( uint64_t payload ) {
    return payload % PRIME; 
}

/** Radix Hash Join**/
void RadixHashJoin( RelationResults & results1, RelationResults & results2, JoinInfo & join, FileArray & fileArray ) {

    // New vector with results after joining
    vector<uint64_t> * joinedResults1 = new vector<uint64_t>;
    vector<uint64_t> * joinedResults2 = new vector<uint64_t>;


    // Use the two vector to create relations structs
    relation relR;
    relR.num_tuples = results1.rowIds->size();
    relR.tuples = new package[relR.num_tuples];
    // initialize the values in relations
    vector<uint64_t> * values1 = fileArray.findColByRowIds( *(results1.rowIds), join.col1, join.rel1 );
    for( int i = 0; i < relR.num_tuples; i++ ) {
        relR.tuples[i].key = (*results1.rowIds)[i];
        relR.tuples[i].payload = (*values1)[i];
    }

    // Use the two vector to create relations structs
    relation relS;
    relS.num_tuples = results2.rowIds->size();
    relS.tuples = new package[relR.num_tuples];
    // initialize the values in relations
    vector<uint64_t> * values2 = fileArray.findColByRowIds( *(results2.rowIds), join.col2, join.rel2 );
    for( int i = 0; i < relS.num_tuples; i++ ) {
        relS.tuples[i].key = (*results2.rowIds)[i];
        relS.tuples[i].payload = (*values2)[i];
    }

 
    // ----------------
    // |  FIRST PART  |
    // ----------------
    // Get the size of each relation
    int Rn = relR.num_tuples;
    int Sn = relS.num_tuples;
    // Get the relationship arrays
    package * R = relR.tuples;
    package * S = relS.tuples;
    // Also create the two new arrays
    package * Rt = new package[Rn];
    package * St = new package[Sn];
    // Generate bucket number
    int n = 3;
    int temp = n;
    int buckets = 1;
    // Calculate 2^n
    while ( temp ) {
        buckets *= 2;
        temp--;
    }

    // Create Histogram for buckets
    unsigned int * histR = new unsigned int[buckets];
	unsigned int * histS = new unsigned int[buckets];
    // Initialize Histogram
    for ( int i = 0; i < buckets; i++ ) {
        histR[i] = 0;
		histS[i] = 0;
    }
    // Fill up histogram with correct data
    for ( int i = 0; i < Rn; i++ ) {
        uint64_t payload = R[i].payload;
        uint64_t bucket = HashFunction1(payload,n);
        // increase appropriate hist counter
        histR[bucket]++;
    }
	for ( int i = 0; i < Sn; i++ ) {
        uint64_t payload = S[i].payload;
        uint64_t bucket = HashFunction1(payload,n);
        // increase appropriate hist counter
        histS[bucket]++;
    }

    // Create Psum Array
    unsigned int * psumR = new unsigned int[buckets];
	unsigned int * psumS = new unsigned int[buckets];
    // Generate Psum data
    for ( int i = 0; i < buckets; i++ ) {
        if ( i == 0 ) {
            psumR[0] = 0;
			psumS[0] = 0;
            continue; 
        }
        psumR[i] = psumR[i-1] + histR[i-1];
		psumS[i] = psumS[i-1] + histS[i-1];
    }

    // Using Psum Array, create the transformed relationships
    for ( int i = 0; i < Rn; i++ ) {
        uint64_t payload = R[i].payload; // payload to write
        uint64_t bucket = HashFunction1(payload,n); // bucket for that payload
		Rt[ psumR[bucket] ].key = R[i].key;
		Rt[ psumR[bucket]++ ].payload = R[i].payload; 
    }
	for ( int i = 0; i < Sn; i++ ) {
        uint64_t payload = S[i].payload; // payload to write
        uint64_t bucket = HashFunction1(payload,n); // bucket for that payload
		St[ psumS[bucket] ].key = S[i].key;
		St[ psumS[bucket]++ ].payload = S[i].payload; 
    }

    // ----------------
    // |  SECOND PART  |
    // ----------------
	//initialiaze index and offsets
	int offsetR = 0;
	int offsetS = 0;
	
	//for loop
	for (temp = 0; temp < buckets; temp++){

        //initialize bucket arrays	
        package  * bucketR = new package[histR[temp]];
        package  * bucketS = new package[histS[temp]];

        for (int i = 0; i < histR[temp]; i++){
            bucketR[i] = Rt[i + offsetR];	
        }
        for (int i = 0; i < histS[temp]; i++){
            bucketS[i] = St[i + offsetS];	
        }

        //initialize bucketArray
        uint64_t* bucketArray = new uint64_t[PRIME];
        for (int i = 0; i < PRIME; i++){
            bucketArray[i] = 0;	
        }	
        uint64_t last;
        //select smallest bucket
        if (histR[temp] <= histS[temp]){
            uint64_t * chain = new uint64_t[histR[temp]];
            for (int i = 0; i < histR[temp]; i++){
                uint64_t bucket = HashFunction2(bucketR[i].payload); // bucket for that payload
                last = bucketArray[bucket];			
                bucketArray[bucket] = i + 1;	
                chain[i] = last;		
            }
            //go to other bucket		
            for (int i = 0; i < histS[temp]; i++){
                uint64_t bucket = HashFunction2(bucketS[i].payload); // bucket for that payload
                if ( (last = bucketArray[bucket]) != 0){    //if there is a hash
                    if (bucketR[last-1].payload == bucketS[i].payload){     //if there is a match
                        //add to result
                        Result * result = new Result;
                        result->key1 = bucketR[last-1].key;
                        result->key2 = bucketS[i].key;
                        joinedResults1->push_back(result->key1);
                        joinedResults2->push_back(result->key2);
                        delete result;
                    }
                    //go to next
                    while (chain[last-1] !=0){
                        last = chain[last-1];
                        if (bucketR[last-1].payload == bucketS[i].payload){
                            //add to result
                            Result * result = new Result;
                            result->key1 = bucketR[last-1].key;
                            result->key2 = bucketS[i].key;
                            joinedResults1->push_back(result->key1);
                            joinedResults2->push_back(result->key2);
                            delete result; 
                        }
                    }    
                }	
            }			
            delete chain;
        }
        else{
            uint64_t * chain = new uint64_t[histS[temp]];
            for (int i = 0; i < histS[temp]; i++){
                uint64_t bucket = HashFunction2(bucketS[i].payload); // bucket for that payload
                uint64_t last = bucketArray[bucket];			
                bucketArray[bucket] = i + 1;	
                chain[i] = last;		
            }
            //go to other bucket		
            for (int i = 0; i < histR[temp]; i++){
                uint64_t bucket = HashFunction2(bucketR[i].payload); // bucket for that payload
                if ( (last = bucketArray[bucket]) != 0){    //if there is a hash
                    if (bucketS[last-1].payload == bucketR[i].payload){     //if there is a match
                        //add to result
                        Result * result = new Result;
                        result->key1 = bucketR[i].key;
                        result->key2 = bucketS[last-1].key;
                        joinedResults1->push_back(result->key1);
                        joinedResults2->push_back(result->key2);
                        delete result;
                    }
                    //go to next
                    while (chain[last-1] !=0){
                        last = chain[last-1];
                        if (bucketS[last-1].payload == bucketR[i].payload){
                            //add to result
                            Result * result = new Result;
                            result->key1 = bucketR[i].key;
                            result->key2 = bucketS[last-1].key;
                            joinedResults1->push_back(result->key1);
                            joinedResults2->push_back(result->key2);
                            delete result;
                        }
                    }    
                }	
            }
            delete chain;        
        }
        offsetR = offsetR + histR[temp];
        offsetS = offsetS + histS[temp];
        delete bucketArray;
        delete bucketS;
        delete bucketR;
    }
	
    // ----------------
    // |  THIRD PART  |
    // ----------------

    // Delete of histograms and psums
    delete histR;
    delete histS;
    delete psumR;
    delete psumS;
    // Delete of final arrays
    delete Rt;
    delete St;

    // Delete of previous rowId vectors
    delete results1.rowIds;
    delete results2.rowIds;
    delete values1;
    delete values2;
    // Replace with our new vectors
    results1.rowIds = joinedResults1;
    results2.rowIds = joinedResults2;

}

void selfJoin( RelationResults & results, JoinInfo & join, FileArray & fileArray ){
    vector<uint64_t> * val1, val2;
    vector<uint64_t> * newRowIds = new vector<uint64_t>;
    val1 = fileArray.findColByRowIds(*(results.rowIds), join.col1, join.rel1);
    val2 = fileArray.findColByRowIds(*(results.rowIds), join.col2, join.rel2);

    for (int i=0; i < results.rowIds->size(); i++){
        if ((*val1)[i] == (*val2)[i]){
            newRowIds->push_back((*results.rowIds)[i]);
        }
    }
    delete(results.rowIds);
    results.rowIds = newRowIds;
    
}

void joinedRelJoin(){
    return;
}

// void crossProduct(){
//     return;
// }