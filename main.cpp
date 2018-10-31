#include <iostream>
#include <cstdint>

using namespace std;

/** Type definition for a tuple */
typedef struct   {
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
* Type definition for a relation.
* It consists of an array of tuples and a size of the relation.
*/
struct result {

};

// Returns the value of the n less significant bits of a payload
char HashFunction( int32_t payload, int n ) {
    return payload & ((1 << n) - 1);
}

/** Radix Hash Join**/
result* RadixHashJoin(relation *relR, relation *relS) {
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
    unsigned int * hist = new unsigned int[buckets];
    // Initialize Histogram
    for ( int i = 0; i < buckets; i++ ) {
        hist[i] = 0;
    }
    // Fill up histogram with correct data
    for ( int i = 0; i < Rn; i++ ) {
        int32_t payload = R[i].payload;
        uint32_t bucket = HashFunction(payload,n);
        // increase appropriate hist counter
        hist[bucket]++;
    }
    // print histogram for debug purposes
    cout << " .:: Histogram ::." << endl;
    for ( int i = 0; i < buckets; i++ ) {
        cout << '\t' << i << " : " << hist[i] << endl;
    }

    // Create Psum Array
    unsigned int * psum = new unsigned int[buckets];
    // Generate Psum data
    for ( int i = 0; i < buckets; i++ ) {
        if ( i == 0 ) {
            psum[0] = 0;
            continue; 
        }
        psum[i] = psum[i-1] + hist[i-1];
    }
    // print Psum for debug purposes
    cout << " .:: Psum ::." << endl;
    for ( int i = 0; i < buckets; i++ ) {
        cout << '\t' << i << " : " << psum[i] << endl;
    }

    // Using Psum Array, create the transformed relationships
    for ( int i = 0; i < Rn; i++ ) {
        int32_t payload = R[i].payload; // payload to write
        uint32_t bucket = HashFunction(payload,n); // bucket for that payload
    }


    // ----------------
    // |  SECOND PART  |
    // ----------------


    // ----------------
    // |  THIRD PART  |
    // ----------------
}



int main ( void ) {

    relation rel;
    rel.num_tuples = 7;
    rel.tuples = new package[7];
    // SAMPLE DATA
    rel.tuples[0].key = 1;
    rel.tuples[0].payload = 0;
    rel.tuples[1].key = 2;
    rel.tuples[1].payload = 0;
    rel.tuples[2].key = 3;
    rel.tuples[2].payload = 3;
    rel.tuples[3].key = 4;
    rel.tuples[3].payload = 3;
    rel.tuples[4].key = 5;
    rel.tuples[4].payload = 7;
    rel.tuples[5].key = 6;
    rel.tuples[5].payload = 6;
    rel.tuples[6].key = 7;
    rel.tuples[6].payload = 7;

    RadixHashJoin(&rel,&rel);
    // end of main
    return 0;
}