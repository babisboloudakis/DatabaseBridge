#include "headers/optimize.hpp"
#include <math.h>

using namespace std;

uint64_t Optimize::cost(FilterInfo & filter, RelInfo & relIn){
    
    uint64_t val = filter.constant;
    uint64_t colSize;
    int relPos;
    int colPos;

    switch( filter.op ) {
            case FilterInfo::FilterOperation::LESS:
                for (int j=0; j < relIn.relStats.size(); j++){ //find col
                    if ( j == filter.col){ //cols are ordered
                        colPos = j;
                        //check if null
                        if (relIn.relStats.at(j).size == 0){
                            return 0;
                        }
                        //boundaries check
                        if (relIn.relStats.at(j).max < val){  // all
                            val = relIn.relStats.at(j).max;
                        }
                        else if (relIn.relStats.at(j).min > val){ //size = 0 
                            for (int n=0; n < relIn.relStats.size(); n++){
                                relIn.relStats.at(n).min = 0;
                                relIn.relStats.at(n).size = 0;
                                relIn.relStats.at(n).distinct = 0;
                                relIn.relStats.at(n).max = 0;
                            }
                            return 0;
                        }
                        colSize = relIn.relStats.at(j).size;
                        relIn.relStats.at(j).size = ((val - relIn.relStats.at(j).min) / (relIn.relStats.at(j).max - relIn.relStats.at(j).min) * relIn.relStats.at(j).size);
                        relIn.relStats.at(j).distinct = ((val - relIn.relStats.at(j).min) / (relIn.relStats.at(j).max - relIn.relStats.at(j).min) * relIn.relStats.at(j).distinct);
                        relIn.relStats.at(j).max = val;
                        break;
                    }
                    else{
                        continue;
                    }  
                }
                //update other col stats
                for (int j=0; j < relIn.relStats.size(); j++){
                    if ( j == filter.col){ //already updated
                        continue; 
                    }
                    else{   //for other cols
                        relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - (relIn.relStats.at(colPos).size / colSize), relIn.relStats.at(j).size / relIn.relStats.at(j).distinct));
                        relIn.relStats.at(j).size = relIn.relStats.at(colPos).size;
                    }
                }
                break;        
                
                
            case FilterInfo::FilterOperation::GREATER:
                for (int j=0; j < relIn.relStats.size(); j++){ //find col
                    if ( j == filter.col){ //cols are ordered
                        colPos = j;
                        //check if null
                        if (relIn.relStats.at(j).size == 0){
                            return 0;
                        }
                        //boundaries check
                        if (relIn.relStats.at(j).min > val){  // all
                            val = relIn.relStats.at(j).min;
                        }
                        else if (relIn.relStats.at(j).max < val){ //size = 0 
                            for (int n=0; n < relIn.relStats.size(); n++){
                                relIn.relStats.at(n).min = 0;
                                relIn.relStats.at(n).size = 0;
                                relIn.relStats.at(n).distinct = 0;
                                relIn.relStats.at(n).max = 0;
                            }
                            return 0;
                        }
                        colSize = relIn.relStats.at(j).size;
                        relIn.relStats.at(j).size = ((relIn.relStats.at(j).max - val) / (relIn.relStats.at(j).max - relIn.relStats.at(j).min) * relIn.relStats.at(j).size);
                        relIn.relStats.at(j).distinct = ((relIn.relStats.at(j).max - val) / (relIn.relStats.at(j).max - relIn.relStats.at(j).min) * relIn.relStats.at(j).distinct);
                        relIn.relStats.at(j).min = val;
                        break;
                    }
                    else{
                        continue;
                    }  
                }
                //update other col stats
                for (int j=0; j < relIn.relStats.size(); j++){
                    if ( j == filter.col){ //already updated
                        continue; 
                    }
                    else{   //for other cols
                        relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - (relIn.relStats.at(colPos).size / colSize), relIn.relStats.at(j).size / relIn.relStats.at(j).distinct));
                        relIn.relStats.at(j).size = relIn.relStats.at(colPos).size;
                    }
                }
                break;
            case FilterInfo::FilterOperation::EQUAL:
                for (int j=0; j < relIn.relStats.size(); j++){ //find col
                    if ( j == filter.col){ //cols are ordered
                        colPos = j;
                        //check if null
                        if (relIn.relStats.at(j).size == 0){
                            return 0;
                        }
                        //boundaries check
                        if (relIn.relStats.at(j).min > val || relIn.relStats.at(j).max < val){ //size = 0 
                            for (int n=0; n < relIn.relStats.size(); n++){
                                relIn.relStats.at(n).min = 0;
                                relIn.relStats.at(n).size = 0;
                                relIn.relStats.at(n).distinct = 0;
                                relIn.relStats.at(n).max = 0;
                            }
                            return 0;
                        }
                        colSize = relIn.relStats.at(j).size;
                        relIn.relStats.at(j).size = relIn.relStats.at(j).size/relIn.relStats.at(j).distinct;
                        relIn.relStats.at(j).distinct = 1;
                        relIn.relStats.at(j).max = val;
                        relIn.relStats.at(j).min = val;
                        break;
                    }
                    else{
                        continue;
                    }  
                }
                //update other col stats
                for (int j=0; j < relIn.relStats.size(); j++){
                    if ( j == filter.col){ //already updated
                        continue; 
                    }
                    else{   //for other cols
                        relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - (relIn.relStats.at(colPos).size / colSize), relIn.relStats.at(j).size / relIn.relStats.at(j).distinct));
                        relIn.relStats.at(j).size = relIn.relStats.at(colPos).size;
                    }
                }
                break;
        }   
}

uint64_t Optimize::cost(FilterInfo & filter, vector<RelInfo> & rels){ 
    
    uint64_t val = filter.constant;
    uint64_t colSize;
    int relPos;
    int colPos;

    switch( filter.op ) {
            case FilterInfo::FilterOperation::LESS:
                for (int i=0; i < rels.size(); i++){    //find rel and break
                    if (filter.rel == rels.at(i).rel){
                        relPos = i;
                        for (int j=0; j < rels.at(i).relStats.size(); j++){ //find col
                            
                            if ( j == filter.col){ //cols are ordered
                                colPos = j;
                                //check if null
                                if (rels.at(i).relStats.at(j).size == 0){
                                    return 0;
                                }
                                //boundaries check
                                if (rels.at(i).relStats.at(j).max < val){  // all
                                    val = rels.at(i).relStats.at(j).max;
                                }
                                else if (rels.at(i).relStats.at(j).min > val){ //size = 0 
                                    for (int k=0; k < rels.size(); k++){
                                        for (int n=0; n < rels.at(k).relStats.size(); n++){
                                            rels.at(k).relStats.at(n).min = 0;
                                            rels.at(k).relStats.at(n).size = 0;
                                            rels.at(k).relStats.at(n).distinct = 0;
                                            rels.at(k).relStats.at(n).max = 0;
                                        }
                                    }
                                    return 0;
                                }
                                colSize = rels.at(i).relStats.at(j).size;
                                rels.at(i).relStats.at(j).size = ((val - rels.at(i).relStats.at(j).min) / (rels.at(i).relStats.at(j).max - rels.at(i).relStats.at(j).min) * rels.at(i).relStats.at(j).size);
                                rels.at(i).relStats.at(j).distinct = ((val - rels.at(i).relStats.at(j).min) / (rels.at(i).relStats.at(j).max - rels.at(i).relStats.at(j).min) * rels.at(i).relStats.at(j).distinct);
                                rels.at(i).relStats.at(j).max = val;
                                break;
                            }
                            else{
                                continue;
                            }  
                        }
                        break;
                    }    
                }
                
                for (int i=0; i < rels.size(); i++){    //loop through midresult rels
                    if (filter.rel == rels.at(i).rel){  //if filter rel
                        //update other col stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            if ( j == filter.col){ //already updated
                                continue; 
                            }
                            else{   //for other cols
                                
                                rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - (rels.at(relPos).relStats.at(colPos).size / colSize), rels.at(i).relStats.at(j).size / rels.at(i).relStats.at(j).distinct));
                                rels.at(i).relStats.at(j).size = rels.at(relPos).relStats.at(colPos).size;
                            }   
                        }
                    }
                    else{   //update other rels col stats
                        //update stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - (rels.at(relPos).relStats.at(colPos).size / colSize), rels.at(i).relStats.at(j).size / rels.at(i).relStats.at(j).distinct));
                            rels.at(i).relStats.at(j).size = rels.at(relPos).relStats.at(colPos).size;         
                        }   
                    }    
                }
                break;
            case FilterInfo::FilterOperation::GREATER:
                //
                break;
            case FilterInfo::FilterOperation::EQUAL:
                //
                break;
        }        
}

uint64_t Optimize::cost(JoinInfo & join, vector<RelInfo> & rels1, vector<RelInfo> & rels2){

}

// uint64_t cost(SelfInfo join, vector<RelInfo> rels){

// }

void Optimize::getRelStats(FileArray & fileArray, Parser & parser){
    this->relcnt = parser.relations.size();
    for(int i=0; i<relcnt; i++){
        int colnum = fileArray.getColNum(parser.relations.at(i));
        
        RelInfo temp;
        temp.rel = parser.relations.at(i);
        for(int j=0; j<colnum; j++){
            uint64_t min = fileArray.getColMin(temp.rel, j);
            uint64_t max = fileArray.getColMax(temp.rel, j);
            uint64_t distinct = fileArray.getColUniqueNum(temp.rel, j);
            uint64_t size = fileArray.getRowNum(temp.rel);
            temp.relStats.emplace_back( Stats(temp.rel, j, min, max, size, distinct) );
        }
        
        this->rels.push_back(temp);
        
    }
}

void Optimize::updateStats(vector<FilterInfo> & filters){
    
    
    for (int i=0; i<filters.size(); i++){
        int index;
        for(int j=0; j<relcnt; j++){
            if (this->rels.at(j).rel == filters.at(i).rel){
                index = j;
                break;
            }
        }
        vector<RelInfo> temp;
        temp.push_back(this->rels.at(index));
        this->cost(filters.at(i), temp);
    }


}

// void Optimize::updateStats(vector<SelfInfo> & joins){

// }

void Optimize::optimizeJoins(vector<JoinInfo> & joins){
    vector<JoinInfo> tempJoin = joins;
    //get all matching anad connected doubles
    for (int i=0; i < this->rels.size(); i++){      //loop through rels
        for (int j=0; j < tempJoin.size(); j++){
            if ((tempJoin.at(j).rel1 == this->rels.at(i).rel) || (tempJoin.at(j).rel2 == this->rels.at(i).rel)){        //find join
                MapData tempMap;
                RelInfo tempRel1;
                RelInfo tempRel2;
                int col1;
                int col2;
                //push back joinInfo
                tempMap.joins.push_back(tempJoin.at(j));
                for(int k=0; k < this->rels.size(); k++){       //campture rels to join
                    if (rels.at(k).rel == tempJoin.at(j).rel1){
                        tempRel1 = rels.at(k);
                        col1 = tempJoin.at(j).col1;
                            
                    }
                    else if (rels.at(k).rel == tempJoin.at(j).rel2){
                        tempRel2 = rels.at(k);
                        col2 = tempJoin.at(j).col2;
                        
                    }    
                }
                //alter stats through filter
                if (tempRel1.relStats.at(col1).min < tempRel2.relStats.at(col2).min){
                    //call costFilter on rel1
                    FilterInfo tempFilter(tempRel1.rel, col1, tempRel2.relStats.at(col2).min, FilterInfo::FilterOperation::GREATER);
                    this->cost(tempFilter, tempRel1);

                }
                else if (tempRel1.relStats.at(col1).min > tempRel2.relStats.at(col2).min){
                    //call costFilter on rel2
                    FilterInfo tempFilter(tempRel2.rel, col2, tempRel1.relStats.at(col1).min, FilterInfo::FilterOperation::GREATER);
                    this->cost(tempFilter, tempRel2);
                }

                if (tempRel1.relStats.at(col1).max > tempRel2.relStats.at(col2).max){
                    //call costFilter on rel1
                    FilterInfo tempFilter(tempRel1.rel, col1, tempRel2.relStats.at(col2).max, FilterInfo::FilterOperation::LESS);
                    this->cost(tempFilter, tempRel1);
                }
                else if (tempRel1.relStats.at(col1).max < tempRel2.relStats.at(col2).max){
                    //call costFilter on rel2
                    FilterInfo tempFilter(tempRel2.rel, col2, tempRel1.relStats.at(col1).max, FilterInfo::FilterOperation::LESS);
                    this->cost(tempFilter, tempRel2);
                }
                //push modified rels
                tempMap.joinedRels.push_back(tempRel1);
                tempMap.joinedRels.push_back(tempRel2);
                //update cost
                //tempMap.cost = cost(join, rel1, rel2);
                //push struvt to big map
                this->map.push_back(tempMap);
                //delete joinInfo for no duplicates
                tempJoin.erase(tempJoin.begin() + j);
                j--;
            }
        }
    }

    //get all triples quadruples etc until there are no new joinInfo to be included 

}

void Optimize::optimizeQuery(FileArray & fileArray, Parser & parser){
        
    //1. Get rel stats
    this->getRelStats(fileArray, parser);
    //2. Update rels stats from filters and self join
    if (parser.filters.size() > 0){
        this->updateStats(parser.filters);
    }
    //3. Rearange joins using map structure and update stats
    if (parser.joins.size() > 1){
        this->optimizeJoins(parser.joins);
        //4. return new parser.joins vector (or filters)
    }
    //else do nothing
     









    return;
}