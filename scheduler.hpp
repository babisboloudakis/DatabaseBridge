#include <vector>
#include <iostream>

typedef struct RelationResults {

    int relPos;
    vector<uint64_t> /* * */ rowIds;
    // Constructor for RelationRows
    //RelationRows(relPos,rowIds) : relPos(relPos), rowIds(rowIds) {};

} RelationResults ;

typedef struct JoinInputRelation {
    
}

class Scheduler {

    public:
    // Intermediate results
    vector<RelationResults> results;


    Scheduler(){

    }

    void loadRelations(); // we need that ?!?!
    void ApplyFilters();
    void ApplyJoins();

}