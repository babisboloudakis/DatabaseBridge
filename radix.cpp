#include <iostream>
#include <cstdint>
#include <cstring>

#define BUFFERSIZE 1048576 // 1024 * 1024 = 1 MB
#define PRIME 7 // prime number used in HashFunction2

using namespace std;

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

// Struct for bucket, stores a buffer of 1MB, a pointer
// to the next bucket as well as the offset for free space
// inside the bucket
typedef struct ListNode {
    char buffer[BUFFERSIZE];
    struct ListNode * next;
    int offset;
} ListNode;

// ResultList is used in order to store the results of bridge
// into buckets. It tracks the first and last bucket. It also
// includes a couple methos related to the resultList
class ResultList {

    ListNode * head; // First bucket
    ListNode * current; // Last bucket

    public:
    // Constructor for ResultList, also initializes listNode
    ResultList() {
        this->head = new ListNode;
        // Initialize listNode
        this->head->offset = 0;
        this->head->next = NULL;
        memset(this->head->buffer, 0, BUFFERSIZE);
        this->current = this->head;
    }
    // Destructor for ResultList
    ~ResultList() {
        ListNode * current;
        while ( this->head != NULL ) {
            current = this->head;
            this->head = current->next;
            delete current;
        }
    }

    // Method to insert result into the ResultList, also checks for overflow
    // and handles it
    void * insertResult( Result * result ) {
        ListNode * bucket = this->current;
        if ( sizeof(result) > BUFFERSIZE - bucket->offset ) {
            ListNode * newBucket = new ListNode;
            // Initialize new ListNode
            newBucket->next = NULL;
            newBucket->offset = 0;
            memset(newBucket->buffer, 0, BUFFERSIZE);
            bucket->next = newBucket;
            bucket = newBucket;
            this->current = bucket;
        }
        memcpy(bucket->buffer + bucket->offset, result, sizeof(*result));
        bucket->offset += sizeof(result);
    }

    void * printResult() {
        Result currentResult;
        ListNode * current = this->head;
        cout << "Results are " << endl;
        cout << "^^^^^^^^^^^ " << endl;
        do {
            int index = 0;
            while ( index < current->offset ) {
                memcpy(&currentResult, current->buffer + index, sizeof(Result));
                cout << currentResult.key1 << " <-> " << currentResult.key2 << endl;
                index +=  sizeof(Result);
            }
            current = current->next;
        } while ( current != NULL );
    }

}; // End of resultList class


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
ResultList * RadixHashJoin(relation *relR, relation *relS) {

    ResultList * listR = new ResultList();

    // ----------------
    // |  FIRST PART  |
    // ----------------
    // Get the size of each relation
    int Rn = relR->num_tuples;
    int Sn = relS->num_tuples;
    // Get the relationship arrays
    package * R = relR->tuples;
    package * S =relS->tuples;
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
                        listR->insertResult(result);
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
                            listR->insertResult(result);
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
                        listR->insertResult(result);
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
                            listR->insertResult(result);
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

    return listR;
}


// Main function created in order to test
// our bridge implementation
int main ( void ) {
    // Allocation of two tables R and S
    relation rel;
    rel.num_tuples = 4;
    rel.tuples = new package[rel.num_tuples];
    relation rel2;
    rel2.num_tuples = 3;
    rel2.tuples = new package[rel2.num_tuples];

    // Sample data same with the one in given requirements
    // Table R
    rel.tuples[0].key = 1;
    rel.tuples[0].payload = 1;
    rel.tuples[1].key = 2;
    rel.tuples[1].payload = 2;
    rel.tuples[2].key = 3;
    rel.tuples[2].payload = 3;
    rel.tuples[3].key = 4;
    rel.tuples[3].payload = 4;
    // Table S
    rel2.tuples[0].key = 1;
    rel2.tuples[0].payload = 1;
    rel2.tuples[1].key = 2;
    rel2.tuples[1].payload = 1;
    rel2.tuples[2].key = 3;
    rel2.tuples[2].payload = 3;

    // Call of RHJ with the two relations
    ResultList* list = RadixHashJoin(&rel,&rel2);
    list->printResult();

    // Delete for Result list which is alllocated inside RadixHashJoin
    // Delete of Table R and Table S
    delete list;
    delete rel.tuples;
    delete rel2.tuples;

    return 0;
}