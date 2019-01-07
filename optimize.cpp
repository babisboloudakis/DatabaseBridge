#include "headers/optimize.hpp"
#include <math.h>

using namespace std;

uint64_t cost(FilterInfo filter, vector<RelInfo> rels){ 
    
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
                            if ( j == filter.col){ //cols are ordered
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

void Optimize::optimizeQuery(FileArray & fileArray, Parser & parser){
    //optimize Filters
    int score; //min?
    int index;
    for(int i=0; i<parser.joins.size(); i++){
        uint64_t min1 = fileArray.getColMin(parser.joins[i].rel1, parser.joins[i].col1);
        uint64_t max1 = fileArray.getColMax(parser.joins[i].rel1, parser.joins[i].col1);
        uint64_t min2 = fileArray.getColMin(parser.joins[i].rel2, parser.joins[i].col2);
        uint64_t max2 = fileArray.getColMax(parser.joins[i].rel2, parser.joins[i].col2);
        uint64_t uniqueNum1 = fileArray.getColUniqueNum(parser.joins[i].rel1, parser.joins[i].col1);
        uint64_t uniqueNum2 = fileArray.getColUniqueNum(parser.joins[i].rel2, parser.joins[i].col2);
        uint64_t size1 = fileArray.getRowNum(parser.joins[i].rel1);
        uint64_t size2 = fileArray.getRowNum(parser.joins[i].rel2);
        int temp;
        //select min = size1 + size2
        if (i == 0){
            score = size1 + size2;
            index = 0;
        }
        else{
            temp = size1 + size2;
            if  (temp < score){
                score = temp;
                index = i;
            }
        }
        //MIN 1 > MAX 2 || MIN 2 > MAX 1 => NULL
        if (min1 > max2 || min2 > max1){
            score = 0;
            index = i;
        }
        
    }
    JoinInfo join = parser.joins.at(index);
    parser.joins.erase(parser.joins.begin() + index);
    parser.joins.insert(parser.joins.begin(), join);
    
    return;
}