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

typedef struct RelInfo {
    int rel;
    vector<Stats> relStats;
} RelInfo;


typedef struct MapData {
    vector<JoinInfo> joins;
    vector<RelInfo> joinedRels;
    uint64_t cost;
} MapData;


class Optimize{
    public:

    int relcnt;
    vector<RelInfo> rels;
    // vector<SelfInfo> * selfJoin;
    vector<MapData> map;  

    Optimize(){
        this->relcnt = 0;
    };
    
    // Optimize(int relcnt, vector<FilterInfo> & filters) : relcnt(relcnt){
        
    // };

    ~Optimize(){};
    
    uint64_t cost(FilterInfo & filter, RelInfo & relIn);

    uint64_t cost(FilterInfo & filter, vector<RelInfo> & rels);

    uint64_t cost(JoinInfo & join, vector<RelInfo> & rels1, vector<RelInfo> & rels2);

    //uint64_t cost(SelfInfo filter);
    
    void getRelStats(FileArray & fileArray, Parser & parser);

    void updateStats(vector<FilterInfo> & filters);

    void optimizeJoins(vector<JoinInfo> & joins);

    // void Optimize::updateStats(vector<SelfInfo> & joins);

    void optimizeQuery(FileArray & fileArray, Parser & parser);
};