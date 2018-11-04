#include <iostream>
#include <cstdint>
#include <cstring>

#define BUFFERSIZE 1048576 // 1 MB
#define PRIME 7 // prime number used in HashFunction2

using namespace std;

/** Type definition for a tuple */
typedef struct package {
    int32_t key;
    int32_t payload;
} package;

/**
* Type definition for a relation.
* It consists of an array of tuples and a size of the relation.
*/
typedef struct relation {
    package * tuples;
    uint32_t num_tuples;
} relation;

/**
* Type definition for a result.
* It consists of a tuple of 2 keys for each rowId.
*/
typedef struct Result {
    int32_t key1;
    int32_t key2;
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
        // cout << "Inserted result " << result->key1 << " " << result->key2 << endl;  

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
int HashFunction1( int32_t payload, int n ) {
    return payload & ((1 << n) - 1);
}

// We modulo the payload with a prime number
// so that we get better divisions
int HashFunction2( int32_t payload ) {
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
        int32_t payload = R[i].payload;
        uint32_t bucket = HashFunction1(payload,n);
        // increase appropriate hist counter
        histR[bucket]++;
    }
	for ( int i = 0; i < Sn; i++ ) {
        int32_t payload = S[i].payload;
        uint32_t bucket = HashFunction1(payload,n);
        // increase appropriate hist counter
        histS[bucket]++;
    }

    // print histogram for debug purposes
    cout << " .:: Histogram R ::." << endl;
    for ( int i = 0; i < buckets; i++ ) {
        cout << '\t' << i << " : " << histR[i] << endl;
    }
	// print histogram for debug purposes
    cout << " .:: Histogram S ::." << endl;
    for ( int i = 0; i < buckets; i++ ) {
        cout << '\t' << i << " : " << histS[i] << endl;
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
    // print Psum for debug purposes
    cout << " .:: PsumR ::." << endl;
    for ( int i = 0; i < buckets; i++ ) {
        cout << '\t' << i << " : " << psumR[i] << endl;
    }
	cout << " .:: PsumS ::." << endl;
    for ( int i = 0; i < buckets; i++ ) {
        cout << '\t' << i << " : " << psumS[i] << endl;
    }

    // Using Psum Array, create the transformed relationships
    for ( int i = 0; i < Rn; i++ ) {
        int32_t payload = R[i].payload; // payload to write
        uint32_t bucket = HashFunction1(payload,n); // bucket for that payload
		Rt[ psumR[bucket] ].key = R[i].key;
		Rt[ psumR[bucket]++ ].payload = R[i].payload; 
    }
	for ( int i = 0; i < Sn; i++ ) {
        int32_t payload = S[i].payload; // payload to write
        uint32_t bucket = HashFunction1(payload,n); // bucket for that payload
		St[ psumS[bucket] ].key = S[i].key;
		St[ psumS[bucket]++ ].payload = S[i].payload; 
    }
	
	// print Rt for debug purposes
    cout << " .:: Rt ::." << endl;
    for ( int i = 0; i < Rn; i++ ) {
        cout << '\t' << i << " : " << Rt[i].payload << endl;
    }
	// print Rt for debug purposes
    cout << " .:: St ::." << endl;
    for ( int i = 0; i < Sn; i++ ) {
        cout << '\t' << i << " : " << St[i].payload << endl;
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
        uint32_t* bucketArray = new uint32_t[PRIME];
        for (int i = 0; i < PRIME; i++){
            bucketArray[i] = 0;	
        }	
        uint32_t last;
        //select smallest bucket
        if (histR[temp] <= histS[temp]){
            uint32_t * chain = new uint32_t[histR[temp]];
            for (int i = 0; i < histR[temp]; i++){
                uint32_t bucket = HashFunction2(bucketR[i].payload); // bucket for that payload
                last = bucketArray[bucket];			
                bucketArray[bucket] = i + 1;	
                chain[i] = last;		
            }
            //go to other bucket		
            for (int i = 0; i < histS[temp]; i++){
                uint32_t bucket = HashFunction2(bucketS[i].payload); // bucket for that payload
                if ( (last = bucketArray[bucket]) != 0){    //if there is a hash
                    if (bucketR[last-1].payload == bucketS[i].payload){     //if there is a match
                        //add to result
                        cout << bucketR[last-1].key << " matches " << bucketS[i].key << endl;
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
                            cout << bucketR[last-1].key << " matches " << bucketS[i].key << endl;
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
            uint32_t * chain = new uint32_t[histS[temp]];
            for (int i = 0; i < histS[temp]; i++){
                uint32_t bucket = HashFunction2(bucketS[i].payload); // bucket for that payload
                uint32_t last = bucketArray[bucket];			
                bucketArray[bucket] = i + 1;	
                chain[i] = last;		
            }
            //go to other bucket		
            for (int i = 0; i < histR[temp]; i++){
                uint32_t bucket = HashFunction2(bucketR[i].payload); // bucket for that payload
                if ( (last = bucketArray[bucket]) != 0){    //if there is a hash
                    if (bucketS[last-1].payload == bucketR[i].payload){     //if there is a match
                        //add to result
                        cout << bucketR[i].key << " matches " << bucketS[last-1].key << endl;
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
                            cout << bucketR[i].key << " matches " << bucketS[last-1].key << endl;
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
    // listR.printResult();
    return listR;

}



int main ( void ) {
    // Main function created in order to test
    // our bridge implementation
    relation rel;
    rel.num_tuples = 7;
    rel.tuples = new package[7];
    // SAMPLE DATA
    rel.tuples[0].key = 1;
    rel.tuples[0].payload = 1;

    rel.tuples[1].key = 2;
    rel.tuples[1].payload = 2;

    rel.tuples[2].key = 3;
    rel.tuples[2].payload = 3;

    rel.tuples[3].key = 4;
    rel.tuples[3].payload = 4;

    rel.tuples[4].key = 5;
    rel.tuples[4].payload = 8;

    rel.tuples[5].key = 6;
    rel.tuples[5].payload = 10;

    rel.tuples[6].key = 7;
    rel.tuples[6].payload = 12;

	relation rel2;
    rel2.num_tuples = 5;
    rel2.tuples = new package[5];
    // SAMPLE DATA
    rel2.tuples[0].key = 1;
    rel2.tuples[0].payload = 1;

    rel2.tuples[1].key = 2;
    rel2.tuples[1].payload = 3;

    rel2.tuples[2].key = 3;
    rel2.tuples[2].payload = 4;

    rel2.tuples[3].key = 4;
    rel2.tuples[3].payload = 6;

    rel2.tuples[4].key = 5;
    rel2.tuples[4].payload = 8;
    
    (RadixHashJoin(&rel,&rel))->printResult();
    // end of main
    return 0;

}