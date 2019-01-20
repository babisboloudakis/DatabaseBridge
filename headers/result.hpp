#include <iostream>
#include <vector>

typedef struct RelationResults {
    int relPos;
    int index;
    vector<uint64_t> * rowIds;
} RelationResults ;

typedef struct MidResult {
    vector<RelationResults> * res;
    vector<int> * rels;
    vector<int> * indexs;
} MidResult;