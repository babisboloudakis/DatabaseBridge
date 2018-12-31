#include <iostream>
#include <vector>

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
    //operator =
    inline JoinInfo operator=(JoinInfo a) {
        rel1=a.rel1;
        col1=a.col1;
        rel2=a.rel2;
        col2=a.col2;
        return a;
    }

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
    //operator =
    inline FilterInfo operator=(FilterInfo a) {
        rel=a.rel;
        col=a.col;
        constant=a.constant;
        return a;
    }

} FilterInfo;
static const vector<FilterInfo::FilterOperation> comparisonTypes { FilterInfo::FilterOperation::LESS, FilterInfo::FilterOperation::GREATER, FilterInfo::FilterOperation::EQUAL};