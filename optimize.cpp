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
                    relIn.relStats.at(j).size = (uint64_t)((double)(val - relIn.relStats.at(j).min) / (double)(relIn.relStats.at(j).max - relIn.relStats.at(j).min) * (double)relIn.relStats.at(j).size);
                    relIn.relStats.at(j).distinct = (uint64_t)((double)(val - relIn.relStats.at(j).min) / (double)(relIn.relStats.at(j).max - relIn.relStats.at(j).min) * (double)relIn.relStats.at(j).distinct);
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
                    relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - (double)(relIn.relStats.at(colPos).size / (double)colSize), (double)relIn.relStats.at(j).size / (double)relIn.relStats.at(j).distinct));
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
                    relIn.relStats.at(j).size = (uint64_t)(((double)(relIn.relStats.at(j).max - val) / (double)(relIn.relStats.at(j).max - relIn.relStats.at(j).min)) * (double)relIn.relStats.at(j).size);
                    
                    relIn.relStats.at(j).distinct = (uint64_t)((double)(relIn.relStats.at(j).max - val) / (double)(relIn.relStats.at(j).max - relIn.relStats.at(j).min) * (double)relIn.relStats.at(j).distinct);
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
                    relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)relIn.relStats.at(colPos).size / (double)colSize), (double)relIn.relStats.at(j).size / (double)relIn.relStats.at(j).distinct));
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
                    relIn.relStats.at(j).size = (uint64_t)((double)relIn.relStats.at(j).size/(double)relIn.relStats.at(j).distinct);
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
                    relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)relIn.relStats.at(colPos).size / (double)colSize), (double)relIn.relStats.at(j).size / (double)relIn.relStats.at(j).distinct));
                    relIn.relStats.at(j).size = relIn.relStats.at(colPos).size;
                }
            }
            break;
    }
    return relIn.relStats.at(0).size;  
}

uint64_t Optimize::cost(FilterInfo & filter, vector<RelInfo> & rels){ 
    
    uint64_t val = filter.constant;
    uint64_t colSize;
    int relPos;
    int colPos;

    switch( filter.op ) {
            case FilterInfo::FilterOperation::LESS:
                for (int i=0; i < rels.size(); i++){    //find rel and break
                    if (filter.rel == rels.at(i).rel && filter.index == rels.at(i).index){
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
                                rels.at(i).relStats.at(j).size = (uint64_t)((double)(val - rels.at(i).relStats.at(j).min) / (double)(rels.at(i).relStats.at(j).max - rels.at(i).relStats.at(j).min) * (double)rels.at(i).relStats.at(j).size);
                                rels.at(i).relStats.at(j).distinct = (uint64_t)((double)(val - rels.at(i).relStats.at(j).min) / (double)(rels.at(i).relStats.at(j).max - rels.at(i).relStats.at(j).min) * (double)rels.at(i).relStats.at(j).distinct);
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
                    if (filter.rel == rels.at(i).rel && filter.index == rels.at(i).index){  //if filter rel
                        //update other col stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            if ( j == filter.col){ //already updated
                                continue; 
                            }
                            else{   //for other cols
                                
                                rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)rels.at(relPos).relStats.at(colPos).size / (double)colSize), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                                rels.at(i).relStats.at(j).size = rels.at(relPos).relStats.at(colPos).size;
                            }   
                        }
                    }
                    else{   //update other rels col stats
                        //update stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)rels.at(relPos).relStats.at(colPos).size / (double)colSize), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                            rels.at(i).relStats.at(j).size = rels.at(relPos).relStats.at(colPos).size;         
                        }   
                    }    
                }
                break;
            case FilterInfo::FilterOperation::GREATER:
                for (int i=0; i < rels.size(); i++){    //find rel and break
                    if (filter.rel == rels.at(i).rel && filter.index == rels.at(i).index){
                        relPos = i;
                        for (int j=0; j < rels.at(i).relStats.size(); j++){ //find col
                            
                            if ( j == filter.col){ //cols are ordered
                                colPos = j;
                                //check if null
                                if (rels.at(i).relStats.at(j).size == 0){
                                    return 0;
                                }
                                //boundaries check
                                if (rels.at(i).relStats.at(j).min > val){  // all
                                    val = rels.at(i).relStats.at(j).min;
                                }
                                else if (rels.at(i).relStats.at(j).max < val){ //size = 0 
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
                                rels.at(i).relStats.at(j).size = (uint64_t)((double)(rels.at(i).relStats.at(j).max - val) / (double)(rels.at(i).relStats.at(j).max - rels.at(i).relStats.at(j).min) * (double)rels.at(i).relStats.at(j).size);
                                rels.at(i).relStats.at(j).distinct = (uint64_t)((double)(rels.at(i).relStats.at(j).max - val) / (double)(rels.at(i).relStats.at(j).max - rels.at(i).relStats.at(j).min) * (double)rels.at(i).relStats.at(j).distinct);
                                rels.at(i).relStats.at(j).min = val;
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
                    if (filter.rel == rels.at(i).rel && filter.index == rels.at(i).index){  //if filter rel
                        //update other col stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            if ( j == filter.col){ //already updated
                                continue; 
                            }
                            else{   //for other cols
                                
                                rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)rels.at(i).relStats.at(colPos).size / (double)colSize), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                                rels.at(i).relStats.at(j).size = rels.at(i).relStats.at(colPos).size;
                            }   
                        }
                    }
                    else{   //update other rels col stats
                        //update stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)rels.at(i).relStats.at(colPos).size / (double)colSize), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                            rels.at(i).relStats.at(j).size = rels.at(i).relStats.at(colPos).size;         
                        }   
                    }    
                }
                break;
            case FilterInfo::FilterOperation::EQUAL:
                for (int i=0; i < rels.size(); i++){    //find rel and break
                    if (filter.rel == rels.at(i).rel && filter.index == rels.at(i).index){
                        relPos = i;
                        for (int j=0; j < rels.at(i).relStats.size(); j++){ //find col
                            
                            if ( j == filter.col){ //cols are ordered
                                colPos = j;
                                //check if null
                                if (rels.at(i).relStats.at(j).size == 0){
                                    return 0;
                                }
                                else if (rels.at(i).relStats.at(j).min > val || rels.at(i).relStats.at(j).max < val){ //size = 0 
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
                                rels.at(i).relStats.at(j).size = (uint64_t)((double)rels.at(i).relStats.at(j).size/(double)rels.at(i).relStats.at(j).distinct);
                                rels.at(i).relStats.at(j).distinct = 1;
                                rels.at(i).relStats.at(j).max = val;
                                rels.at(i).relStats.at(j).min = val;
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
                    if (filter.rel == rels.at(i).rel && filter.index == rels.at(i).index){  //if filter rel
                        //update other col stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            if ( j == filter.col){ //already updated
                                continue; 
                            }
                            else{   //for other cols
                                
                                rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)rels.at(i).relStats.at(colPos).size / (double)colSize), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                                rels.at(i).relStats.at(j).size = rels.at(i).relStats.at(colPos).size;
                            }   
                        }
                    }
                    else{   //update other rels col stats
                        //update stats
                        for (int j=0; j < rels.at(i).relStats.size(); j++){
                            rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)rels.at(i).relStats.at(colPos).size / (double)colSize), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                            rels.at(i).relStats.at(j).size = rels.at(i).relStats.at(colPos).size;         
                        }   
                    }    
                }
                break;
        }
    return rels.at(0).relStats.at(0).size;        
}

uint64_t Optimize::cost(JoinInfo & join, vector<RelInfo> & rels1, vector<RelInfo> & rels2){
    Stats relStats1, relStats2;
    int pos1, pos2; 
    uint64_t n, d, s;
    //if on rel is empty -> all empty
    if(rels1.at(0).relStats.at(0).size == 0 || rels2.at(0).relStats.at(0).size == 0 ){
        for (int i=0; i<rels1.size(); i++){    
            for (int j=0; j<rels1.at(i).relStats.size(); j++){
                rels1.at(i).relStats.at(j).min = 0;
                rels1.at(i).relStats.at(j).size = 0;
                rels1.at(i).relStats.at(j).distinct = 0;
                rels1.at(i).relStats.at(j).max = 0;    
            }      
        }
        for (int i=0; i<rels2.size(); i++){
            for (int j=0; j<rels2.at(i).relStats.size(); j++){
                rels2.at(i).relStats.at(j).min = 0;
                rels2.at(i).relStats.at(j).size = 0;
                rels2.at(i).relStats.at(j).distinct = 0;
                rels2.at(i).relStats.at(j).max = 0; 
            }
        } 
        return 0;   
    }
    for (int i=0; i<rels1.size(); i++){
        if (rels1.at(i).rel == join.rel1 && rels1.at(i).index == join.index1){
            pos1 = i;
            for (int j=0; j<rels1.at(i).relStats.size(); j++){
                if (j == join.col1){
                    relStats1 = rels1.at(i).relStats.at(j);
                    break;
                }
                
            }
            break;
        }
    }
    for (int i=0; i<rels2.size(); i++){
        if (rels2.at(i).rel == join.rel2 && rels2.at(i).index == join.index2){
            pos2 = i;
            for (int j=0; j<rels2.at(i).relStats.size(); j++){
                if (j == join.col2){
                    relStats2 = rels2.at(i).relStats.at(j);
                    break;
                }
                
            }
            break;
        } 
    }

    n = relStats1.max - relStats2.min +1;
    d = (uint64_t)((double)(relStats1.distinct * relStats2.distinct) / (double)n);
    s = (uint64_t)((double)(relStats1.size * relStats2.size) / (double)n);

    for (int i=0; i<rels1.size(); i++){
        if (pos1 == i){
            for (int j=0; j<rels1.at(i).relStats.size(); j++){
                if (j == join.col1){
                   rels1.at(i).relStats.at(j).distinct = d;
                   rels1.at(i).relStats.at(j).size = s;
                }
                else{
                    rels1.at(i).relStats.at(j).distinct = rels1.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)d / (double)relStats1.distinct), (double)rels1.at(i).relStats.at(j).size / (double)rels1.at(i).relStats.at(j).distinct));
                    rels1.at(i).relStats.at(j).size = s;
                }
            }
        }
        else{
            for (int j=0; j<rels1.at(i).relStats.size(); j++){
                rels1.at(i).relStats.at(j).distinct = rels1.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)d / (double)relStats1.distinct), (double)rels1.at(i).relStats.at(j).size / (double)rels1.at(i).relStats.at(j).distinct));
                rels1.at(i).relStats.at(j).size = s;
            }
        }
    }
    for(int i=0; i<rels2.size(); i++){
        if (pos2 == i){
            for (int j=0; j<rels2.at(i).relStats.size(); j++){
                if (j == join.col2){
                   rels2.at(i).relStats.at(j).distinct = d;
                   rels2.at(i).relStats.at(j).size = s;
                }
                else{
                    rels2.at(i).relStats.at(j).distinct = rels2.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)d / (double)relStats2.distinct), (double)rels2.at(i).relStats.at(j).size / (double)rels2.at(i).relStats.at(j).distinct));
                    rels2.at(i).relStats.at(j).size = s;
                }
                
            }
        }
        else{
            for (int j=0; j<rels2.at(i).relStats.size(); j++){
                rels2.at(i).relStats.at(j).distinct = rels2.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)d / (double)relStats2.distinct), (double)rels2.at(i).relStats.at(j).size / (double)rels2.at(i).relStats.at(j).distinct));
                rels2.at(i).relStats.at(j).size = s;
            }
        }
    }
    return rels1.at(0).relStats.at(0).size;   
}

uint64_t Optimize::cost(JoinInfo & join, RelInfo & rels1, RelInfo & rels2){
    Stats relStats1, relStats2; 
    uint64_t n, d, s;

    //if on rel is empty -> all empty
    if(rels1.relStats.at(0).size == 0 || rels2.relStats.at(0).size == 0 ){
        for (int j=0; j<rels1.relStats.size(); j++){
            rels1.relStats.at(j).min = 0;
            rels1.relStats.at(j).size = 0;
            rels1.relStats.at(j).distinct = 0;
            rels1.relStats.at(j).max = 0;    
        }      
    
        for (int j=0; j<rels2.relStats.size(); j++){
            rels2.relStats.at(j).min = 0;
            rels2.relStats.at(j).size = 0;
            rels2.relStats.at(j).distinct = 0;
            rels2.relStats.at(j).max = 0; 
        }
        return 0;   
    }
    if (rels1.rel == join.rel1 && rels1.index == join.index1){ 
        for (int j=0; j<rels1.relStats.size(); j++){
            if (j == join.col1){
                relStats1 = rels1.relStats.at(j);
                break;
            } 
        }
        
    }
    
    if (rels2.rel == join.rel2 && rels2.index == join.index2){ 
        for (int j=0; j<rels2.relStats.size(); j++){
            if (j == join.col2){
                relStats2 = rels2.relStats.at(j);
                break;
            }
        }    
    }
    
    n = relStats1.max - relStats1.min +1;
    
    d = (uint64_t)((double)(relStats1.distinct * relStats2.distinct) / (double)n);
    s = (uint64_t)((double)(relStats1.size * relStats2.size) / (double)n);
    
    for (int j=0; j<rels1.relStats.size(); j++){
        if (j == join.col1){
            rels1.relStats.at(j).distinct = d;
            rels1.relStats.at(j).size = s;
        }
        else{
            rels1.relStats.at(j).distinct = rels1.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)d / (double)relStats1.distinct), (double)rels1.relStats.at(j).size / (double)rels1.relStats.at(j).distinct));
            rels1.relStats.at(j).size = s;
        }
    }
    for (int j=0; j<rels2.relStats.size(); j++){
        if (j == join.col2){
            rels2.relStats.at(j).distinct = d;
            rels2.relStats.at(j).size = s;
        }
        else{
            rels2.relStats.at(j).distinct = rels2.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)d / (double)relStats2.distinct), (double)rels2.relStats.at(j).size / (double)rels2.relStats.at(j).distinct));
            rels2.relStats.at(j).size = s;
        } 
    }
    return rels1.relStats.at(0).size;         
}

uint64_t Optimize::cost(JoinInfo & join, vector<RelInfo> & rels){
    int pos1 = -1;
    int pos2 = -1;
    uint64_t min;
    uint64_t max;
    uint64_t n;
    uint64_t size;
    uint64_t fa;
    if (rels.at(0).relStats.at(0).size == 0){
        return 0;
    }
    for(int i=0; i<rels.size(); i++){

        if (join.index1 == rels.at(i).index){
            pos1 = i;
        }
        else if(join.index2 == rels.at(i).index){
            pos2 = i;
        }
    }
    if (rels.at(pos1).relStats.at(join.col1).min > rels.at(pos2).relStats.at(join.col2).min ){
        min = rels.at(pos1).relStats.at(join.col1).min;
    }
    else{
        min = rels.at(pos2).relStats.at(join.col2).min;
    }
    if (rels.at(pos1).relStats.at(join.col1).max < rels.at(pos2).relStats.at(join.col2).max ){
        max = rels.at(pos1).relStats.at(join.col1).max;
    }
    else{
        max = rels.at(pos2).relStats.at(join.col2).max;
    }
    n=max-min+1;
    fa= (uint64_t)((double)rels.at(pos1).relStats.at(join.col1).size/(double)n);
    size=rels.at(pos1).relStats.at(join.col1).size;
    for (int i=0; i<rels.size(); i++){
        if (i == pos1){  //self rel1
            for (int j=0; j<rels.at(i).relStats.size(); j++){
                if (j == join.col1 || j == join.col2){
                    rels.at(i).relStats.at(j).min = min; 
                    rels.at(i).relStats.at(j).max = max;
                    rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                    rels.at(i).relStats.at(j).size = fa;
                    
                }
                else{
                    rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                    rels.at(i).relStats.at(j).size = fa; 
                }
            }
        }
        else if (i == pos2){  //self rel2
            for (int j=0; j<rels.at(i).relStats.size(); j++){
                if (j == join.col1 || j == join.col2){
                    rels.at(i).relStats.at(j).min = min; 
                    rels.at(i).relStats.at(j).max = max;
                    rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                    rels.at(i).relStats.at(j).size = fa;
                    
                }
                else{
                    rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                    rels.at(i).relStats.at(j).size = fa; 
                }
            }
        }
        else{
            for (int j=0; j<rels.at(i).relStats.size(); j++){
                rels.at(i).relStats.at(j).distinct = rels.at(i).relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)rels.at(i).relStats.at(j).size / (double)rels.at(i).relStats.at(j).distinct));
                rels.at(i).relStats.at(j).size = fa;
            }
        }
    }
    return rels.at(0).relStats.at(0).size;   
}

uint64_t Optimize::cost(SelfInfo & join, RelInfo & relIn){
    uint64_t min;
    uint64_t max;
    uint64_t n;
    uint64_t size;
    uint64_t fa;
     if (relIn.relStats.at(0).size == 0){
        return 0;
    }
    if (relIn.relStats.at(join.col1).min > relIn.relStats.at(join.col2).min ){
        min = relIn.relStats.at(join.col1).min;
    }
    else{
        min = relIn.relStats.at(join.col2).min;
    }
    if (relIn.relStats.at(join.col1).max < relIn.relStats.at(join.col2).max ){
        max = relIn.relStats.at(join.col1).max;
    }
    else{
        max = relIn.relStats.at(join.col2).max;
    }
    n = max-min+1;
    fa = (uint64_t)((double)relIn.relStats.at(join.col1).size/(double)n);
    size = relIn.relStats.at(join.col1).size;

    
    for (int j=0; j<relIn.relStats.size(); j++){
        if (j == join.col1 || j == join.col2){
            relIn.relStats.at(j).min = min; 
            relIn.relStats.at(j).max = max;
            relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)relIn.relStats.at(j).size / (double)relIn.relStats.at(j).distinct));
            relIn.relStats.at(j).size = fa;
            
        }
        else{
            relIn.relStats.at(j).distinct = relIn.relStats.at(j).distinct * (1 - (uint64_t)pow(1 - ((double)fa / (double)size), (double)relIn.relStats.at(j).size / (double)relIn.relStats.at(j).distinct));
            relIn.relStats.at(j).size = fa; 
        }
    }
    
   return relIn.relStats.at(0).size;     
}

void Optimize::getRelStats(FileArray & fileArray, Parser & parser){
    this->relcnt = parser.relations.size();
    for(int i=0; i<relcnt; i++){
        int colnum = fileArray.getColNum(parser.relations.at(i));
        
        RelInfo temp;
        temp.rel = parser.relations.at(i);
        temp.index = i;
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

void Optimize::updateStats(vector<FilterInfo> & filters){      //can be improved
    
    
    for (int i=0; i<filters.size(); i++){
        int index;
        for(int j=0; j<relcnt; j++){
            if (this->rels.at(j).rel == filters.at(i).rel && this->rels.at(j).index == filters.at(i).index){
                index = j;
                break;
            }
        }
        this->cost(filters.at(i), this->rels.at(index));
    }


}

void Optimize::updateStats(vector<SelfInfo> & joins){
    for (int i=0; i<joins.size(); i++){
        int index;
        for(int j=0; j<relcnt; j++){
            if (this->rels.at(j).rel == joins.at(i).rel && this->rels.at(j).index == joins.at(i).index){
                index = j;
                break;
            }
        }
        this->cost(joins.at(i), this->rels.at(index));
    }
}

void Optimize::optimizeJoins(vector<JoinInfo> & joins){
    // vector<MapData> map;
    
    vector<JoinInfo> tempJoin = joins;
    //get all matching anad connected doubles
    uint64_t mincost;
    int id;
    MapData minMap;
    for (int i=0; i < this->rels.size(); i++){      //loop through rels
        for (int j=0; j < tempJoin.size(); j++){
            if ((tempJoin.at(j).rel1 == this->rels.at(i).rel && this->rels.at(i).index == tempJoin.at(j).index1) || (tempJoin.at(j).rel2 == this->rels.at(i).rel && this->rels.at(i).index == tempJoin.at(j).index2)){        //find join
                MapData tempMap;
                RelInfo tempRel1;
                RelInfo tempRel2;
                int col1;
                int col2;
                //push back joinInfo
                tempMap.joins.push_back(tempJoin.at(j));
                for(int k=0; k < this->rels.size(); k++){       //campture rels to join
                    if (tempJoin.at(j).rel1 == this->rels.at(k).rel && this->rels.at(k).index == tempJoin.at(j).index1){
                        tempRel1.rel = rels.at(k).rel;
                        tempRel1.index = rels.at(k).index;
                        tempRel1.relStats = rels.at(k).relStats;
                        col1 = tempJoin.at(j).col1;
                            
                    }
                    else if (tempJoin.at(j).rel2 == this->rels.at(k).rel && this->rels.at(k).index == tempJoin.at(j).index2){
                        tempRel2.rel = rels.at(k).rel;
                        tempRel2.index = rels.at(k).index;
                        tempRel2.relStats = rels.at(k).relStats;
                        col2 = tempJoin.at(j).col2;
                        
                    }    
                }
                //alter stats through filter
                if (tempRel1.relStats.at(col1).min < tempRel2.relStats.at(col2).min){
                    //call costFilter on rel1
                    FilterInfo tempFilter(tempRel1.rel, col1, tempRel2.relStats.at(col2).min, FilterInfo::FilterOperation::GREATER, tempRel1.index);
                    this->cost(tempFilter, tempRel1);

                }
                else {
                    //call costFilter on rel2
                    FilterInfo tempFilter(tempRel2.rel, col2, tempRel1.relStats.at(col1).min, FilterInfo::FilterOperation::GREATER, tempRel1.index);
                    this->cost(tempFilter, tempRel2);
                }

                if (tempRel1.relStats.at(col1).max > tempRel2.relStats.at(col2).max){
                    //call costFilter on rel1
                    FilterInfo tempFilter(tempRel1.rel, col1, tempRel2.relStats.at(col2).max, FilterInfo::FilterOperation::LESS, tempRel1.index);
                    this->cost(tempFilter, tempRel1);
                }
                else {
                    //call costFilter on rel2
                    FilterInfo tempFilter(tempRel2.rel, col2, tempRel1.relStats.at(col1).max, FilterInfo::FilterOperation::LESS, tempRel1.index);
                this->cost(tempFilter, tempRel2);
                }
                
                tempMap.cost = cost(tempJoin.at(j), tempRel1, tempRel2);
                
                //push modified rels
                tempMap.joinedRels.push_back(tempRel1);
                tempMap.joinedRels.push_back(tempRel2);
                //push struvt to big map
                
                if (minMap.joins.size() == 0){
                    minMap = tempMap;
                    mincost = tempMap.cost;
                }
                else if(mincost > tempMap.cost){
                    minMap = tempMap;
                    mincost = tempMap.cost;
                }
                //delete joinInfo for no duplicates
                tempJoin.erase(tempJoin.begin() + j);
                j--;
            }
        }
    }

    int cnt = 1;
    //get all triples quadruples etc until there are no new joinInfo to be included 
    
    while(cnt < joins.size()){
        MapData tempMap = minMap;
        MapData map;
        for (int i=0; i<joins.size(); i++){
            int fl = 0;
            for(int j=0; j<tempMap.joins.size(); j++){
                //if same join -> skip
                if (tempMap.joins.at(j).index1 == joins.at(i).index1 && tempMap.joins.at(j).index2 == joins.at(i).index2 && tempMap.joins.at(j).col1 == joins.at(i).col1 && tempMap.joins.at(j).col2 == joins.at(i).col2){
                    fl = 1;
                    break;
                }
            }
            if (fl) continue;
            //if not skipped
            
            int joinid2 = -1;
            for(int j=0; j<tempMap.joinedRels.size(); j++){
                //if rel in common
                if (tempMap.joinedRels.at(j).index == joins.at(i).index1){
                    joinid2 = joins.at(i).index2;
                    break;
                }
                else if (tempMap.joinedRels.at(j).index == joins.at(i).index2){
                    joinid2 = joins.at(i).index1;
                    break;
                }
            }
            if (joinid2 != -1){
                int flag = 0;
                vector<RelInfo> rel;
                for (int n=0; n < tempMap.joinedRels.size(); n++){
                    //check if rel already in mid results
                    if (tempMap.joinedRels.at(n).index == joinid2){
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1){
                    //self join
                    if(map.joins.size() == 0){
                        map=tempMap;
                        map.cost = this->cost(joins.at(i), map.joinedRels);
                        map.joins.push_back(joins.at(i));
                        minMap=map;
                    }
                    else{
                        map=tempMap;
                        map.cost = this->cost(joins.at(i), map.joinedRels);
                        map.joins.push_back(joins.at(i));
                        if(minMap.cost > map.cost){
                            minMap=map;   
                        }
                    }
                }
                else{
                    //normal join
                    if(map.joins.size() == 0){
                        map=tempMap;
                        rel.push_back(this->rels.at(joinid2));
                        map.cost += this->cost(joins.at(i), map.joinedRels, rel);
                        map.joins.push_back(joins.at(i));
                        map.joinedRels.push_back(rel.at(0));
                        minMap=map;
                    }
                    else{
                        map=tempMap;
                        rel.push_back(this->rels.at(joinid2));
                        map.cost += this->cost(joins.at(i), map.joinedRels, rel);
                        map.joins.push_back(joins.at(i));
                        map.joinedRels.push_back(rel.at(0));
                        if(minMap.cost > map.cost){
                            minMap=map;
                        }
                    }
                }
            }    
        }
        cnt++;
    }
    joins=minMap.joins;
}

void Optimize::optimizeQuery(FileArray & fileArray, Parser & parser){
    if (parser.joins.size() > 1){    
        //1. Get rel stats
        this->getRelStats(fileArray, parser);
        //2. Update rels stats from filters and self join
        if (parser.filters.size() > 0){
            this->updateStats(parser.filters);
        }
        if (parser.selfs.size() > 0){
            this->updateStats(parser.selfs);
        }
        //3. Rearange joins using map structure and update stats
        this->optimizeJoins(parser.joins);
        //4. return new parser.joins vector
        
    }
    //else do nothing
    return;
}