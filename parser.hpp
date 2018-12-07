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


typedef struct JoinInfo {
    // struct that contains info for join
    uint64_t rel1;
    uint64_t col1;
    uint64_t rel2;
    uint64_t col2;

    // Struct constructor
    JoinInfo(uint64_t rel1,uint64_t col1,uint64_t rel2,
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
    std::vector<FilterInfo> filters;
    std::vector<JoinInfo> joins;

    public:
    Parser(){};
    ~Parser(){};

    // parse rel col pair < x.y >
    void parseRelCol( std::string & rawPair );
    void parsePredicate( std::string & predicate );
    SelectInfo parsePair ( std::string & rawPair );

    void parseRelations( std::string & rawSelections );
    void parsePredicates( std::string & rawPredicates );
    void parseProjections( std::string & rawProjections );
    void parseQuery( std::string & rawQuery );

    // print method
    void printParseInfo();
};