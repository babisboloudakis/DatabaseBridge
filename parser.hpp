#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
// #include "filter.hpp"
#include "read.hpp"

using namespace std;

typedef struct SelectInfo { 
    // struct that contains info for select, relation.col
    int rel;
    uint64_t col;
    // Struct constructor
    SelectInfo(int rel, uint64_t col) : rel(rel), col(col) {};

} SelectInfo;


typedef struct JoinInfo {
    // struct that contains info for join
    int rel1;
    uint64_t col1;
    int rel2;
    uint64_t col2;

    // Struct constructor
    JoinInfo(int rel1,uint64_t col1,int rel2,
    uint64_t col2) : rel1(rel1), col1(col1), rel2(rel2), col2(col2){};

} JoinInfo;

typedef struct FilterInfo {

    enum FilterOperation : char {
        // enumeration of all filter operations
        LESS = '<',
        GREATER = '>',
        EQUAL = '='
    };

    // info about a filter operation
    int rel;
    uint64_t col;
    uint64_t constant;
    FilterOperation op;

    // Constructor for filterInfo
    FilterInfo(int rel, uint64_t col, uint64_t constant, FilterOperation op) : rel(rel), col(col), constant(constant), op(op) {};

} FilterInfo;
static const vector<FilterInfo::FilterOperation> comparisonTypes { FilterInfo::FilterOperation::LESS, FilterInfo::FilterOperation::GREATER, FilterInfo::FilterOperation::EQUAL};

typedef struct RelationResults {

    int relPos;
    vector<uint64_t> * rowIds;

} RelationResults ;

typedef struct MidResult {
    vector<RelationResults> * res;
    vector<int> * rels;
    //...
} MidResult;
 
void filterResults( RelationResults & results, FilterInfo filter, FileArray & fileArray );

MidResult * RadixHashJoin( MidResult & results1, MidResult & results2, JoinInfo & join, FileArray & fileArray );

class Parser {

    public:
    // Vector of selections
    vector<int> relations;
    vector<SelectInfo> selections;
    vector<FilterInfo> filters;
    vector<JoinInfo> joins;
    //midreasult
    vector<MidResult> midResults;    


    public:
    Parser(){}; 
    ~Parser(){
        for (int i=0; i < this->midResults.size(); i++){
            if (this->midResults[i].res != NULL){
                for (int j=0; j < this->midResults[i].res->size(); j++){
                    if ( (*(this->midResults[i].res))[j].rowIds != NULL){
                        delete ((*(this->midResults[i].res))[j].rowIds);    
                    }
                }
                delete(this->midResults[i].res);
            }
            if (this->midResults[i].rels != NULL){
                delete(this->midResults[i].rels); 
            }
        }    
    }; //delete pointers

    //fnct -> compute(fileArray, string){ parseQuery , prakseis, putresult }
    void computeQuery(FileArray & fileArray, string & line);
    //private
    //fnct -> prakseis((min, max)fliters->midresults epilogh twn joins->joins diaxeirish mid result)
    void optimize(FileArray & fileArray);
    void computeFilters(FileArray & fileArray);
    void computeJoins(FileArray & fileArray);
    void printResult(FileArray & fileArray);
    //applyfiter(&RelationResults, col, &fileArray ) (vector[2],f)
    //applyjoin to diff()
    //apply join to same()

    // parse rel col pair < x.y >
    void parseRelCol( string & rawPair );
    void parsePredicate( string & predicate );
    SelectInfo parsePair ( string & rawPair );

    void parseRelations( string & rawSelections );
    void parsePredicates( string & rawPredicates );
    void parseProjections( string & rawProjections );
    void parseQuery( string & rawQuery );

    // print method
    void printParseInfo();
};