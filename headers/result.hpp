#include <iostream>
#include <vector>

typedef struct RelationResults {

    int relPos;
    vector<uint64_t> * rowIds;

} RelationResults ;

typedef struct MidResult {
    vector<RelationResults> * res;
    vector<int> * rels;
    //...
} MidResult;