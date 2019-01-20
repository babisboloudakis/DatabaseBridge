#ifndef PARSER_INFO_H
#define PARSER_INFO_H
#include "parser_info.hpp"
#endif

#ifndef RESULT_H
#define RESULT_H
#include "result.hpp"
#endif

#ifndef READ_H
#define READ_H
#include "read.hpp"
#endif

#define PRIME 499 // prime number used in HashFunction2 

/** Type definition for a tuple */
typedef struct package {
    uint64_t key;
    uint64_t payload;
    int index;

    inline package operator=(package a) {
        key=a.key;
        payload=a.payload;
        index=a.index;
        return a;
    }
} package;

/**
* Type definition for a relation.
* It consists of an array of tuples and a size of the relation.
*/
typedef struct relation {
    package * tuples;
    uint64_t num_tuples;
} relation;

int HashFunction1( uint64_t payload, int n );
int HashFunction2(uint64_t payload);

class Join{
    
    public:
    Join(){};
    ~Join(){};

    MidResult * join( MidResult & results1, MidResult & results2, JoinInfo & join, FileArray & fileArray );

    void join(MidResult & results, JoinInfo & join, FileArray & fileArray );
};