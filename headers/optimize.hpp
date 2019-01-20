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
    Stats (int rel=0, int col=0, uint64_t min=0, 
    uint64_t max=0, uint64_t size=0, uint64_t distinct=0) : 
    rel(rel), col(col), min(min), max(max), size(size), distinct(distinct) {};

    inline Stats operator=(Stats a) {
        rel=a.rel;
        col=a.col;
        min=a.min;
        max = a.max;
        size = a.size;
        distinct = a.distinct;
        return a;
    }


} Stats;

typedef struct RelInfo {
    int rel;
    int index;
    vector<Stats> relStats;
} RelInfo;


typedef struct MapData {
    vector<JoinInfo> joins;
    vector<RelInfo> joinedRels;
    uint64_t cost;

    inline MapData operator=(MapData a) {
        joins=a.joins;
        joinedRels=a.joinedRels;
        cost=a.cost;
        return a;
    }
} MapData;


class Optimize{
    public:

    int relcnt;
    vector<RelInfo> rels;  

    Optimize(){
        this->relcnt = 0;
    };
    
    // Optimize(int relcnt, vector<FilterInfo> & filters) : relcnt(relcnt){
        
    // };

    ~Optimize(){};
    
    uint64_t cost(FilterInfo & filter, RelInfo & relIn);

    uint64_t cost(FilterInfo & filter, vector<RelInfo> & rels);

    uint64_t cost(JoinInfo & join, vector<RelInfo> & rels1, vector<RelInfo> & rels2);

    uint64_t cost(JoinInfo & join, RelInfo & rels1, RelInfo & rels2);
    
    uint64_t cost(JoinInfo & join, vector<RelInfo> & rels);

    uint64_t cost(SelfInfo & join, RelInfo & relIn);

    uint64_t cost_a(JoinInfo & join, vector<RelInfo> & rels1, vector<RelInfo> & rels2);
    
    uint64_t cost_a(JoinInfo & join, RelInfo & rels1, RelInfo & rels2);
    
    void getRelStats(FileArray & fileArray, Parser & parser);

    void updateStats(vector<FilterInfo> & filters);

    void updateStats(vector<SelfInfo> & joins);

    void optimizeJoins(vector<JoinInfo> & joins);

    void optimizeQuery(FileArray & fileArray, Parser & parser);
};