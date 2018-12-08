#include <vector>
#include <iostream>

typedef struct RelationRows {

    int relPos;
    vector<uint64_t> * rowIds;
    // Constructor for RelationRows
    RelationRows(relPos,rowIds) : relPos(relPos), rowIds(rowIds) {};

} RelationRows ;

class Scheduler {

    public:
    // Intermediate results
    vector<RelationRows> results;

    void loadRelations(); // we need that ?!?!
    void ApplyFilters();
    void ApplyJoins();

}