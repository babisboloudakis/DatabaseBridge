#ifndef READ_H
#define READ_H
#include "read.hpp"
#endif

#include "parser.hpp"


typedef struct Stats {
    int rel;
    int col;
    uint64_t min;
    uint64_t max;
    uint64_t size;
    uint64_t distinct;
    
    // Struct constructor
    Stats (int rel, int col, uint64_t min, 
    uint64_t max, uint64_t size, uint64_t distinct) : 
    rel(rel), col(col), min(min), max(max), size(size), distinct(distinct) {};

} Stats;

typedef struct relInfo {
    int rel;
    vector<Stats> * relStats;
} relInfo;



class Optimize{
    public:

    int relcnt;
    vector<relInfo> rels;

    Optimize(int relcnt, FilterInfo & filter) : relcnt(relcnt){

    };
    ~Optimize(){};

    void optimizeQuery(FileArray & fileArray, Parser & parser);
};