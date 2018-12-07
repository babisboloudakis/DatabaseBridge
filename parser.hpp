#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>

typedef struct SelectInfo { 

    // struct that contains info for select, relation.col
    uint64_t rel;
    uint64_t col;
    // Struct constructor
    SelectInfo(uint64_t rel, uint64_t col) : rel(rel), col(col) {};

} SelectInfo;


typedef struct FilterInfo {

    enum FilterOperation : char {
        // enumeration of all filter operations
        LESS = '<',
        GREATER = '>',
        EQUAL = '='
    };

    // info about a filter operation
    uint64_t rel;
    uint64_t col;
    uint64_t constant;
    FilterOperation op;

    // Constructor for filterInfo
    FilterInfo(uint64_t rel, uint64_t col, uint64_t constant, FilterOperation op) : rel(rel), col(col), constant(constant), op(op) {};

} FilterInfo;
static const std::vector<FilterInfo::FilterOperation> comparisonTypes { FilterInfo::FilterOperation::LESS, FilterInfo::FilterOperation::GREATER, FilterInfo::FilterOperation::EQUAL};


class Parser {

    public:
    // Vector of selections
    std::vector<uint64_t> relations;
    std::vector<SelectInfo> selections;
    

    public:
    Parser(){};
    ~Parser(){};

    // parse rel col pair < x.y >
    void parseRelCol( std::string & rawPair );
    void parsePredicate( std::string & predicate );

    void parseRelations( std::string & rawSelections );
    void parsePredicates( std::string & rawPredicates );
    void parseProjections( std::string & rawProjections );
    void parseQuery( std::string & rawQuery );
};