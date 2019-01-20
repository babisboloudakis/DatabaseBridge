#include "headers/query.hpp"
#include <sstream>

using namespace std;

void Query::printResult(FileArray &fileArray){
    if (this->midResults.size() > 1){
        cerr << "Cant Process CrossProduct" << endl;
        cout << "NULL" << endl;
        return;
    }
    else if (this->midResults.size() == 1){
        for(int i=0; i<this->parser.selections.size(); i++){
            int index=-1;
            vector<uint64_t> * val;
            uint64_t sum=0;
            if (this->midResults.front().indexs != NULL){
                for (int k=0; k < this->midResults.front().indexs->size(); k++){    
                    if ( (*(this->midResults.front().indexs))[k] == this->parser.selections[i].index){
                        index=k;
                        break;    
                    }
                }
                if (index < 0){
                    cerr << "Selection Relation doesnt exist in midResult";
                    cout << endl;
                    return;
                }
                if (this->midResults.front().res->at(index).rowIds->size() == 0){
                    if (i == this->parser.selections.size() - 1)
                    {
                        cout << "NULL";
                    }
                    else
                    {
                        cout << "NULL" << " ";
                    }
                    continue;
                }
                val = fileArray.findColByRowIds(*(this->midResults.front().res->at(index).rowIds), this->parser.selections[i].col, this->midResults.front().res->at(index).relPos);
                if (val == NULL){
                    cerr << "Error in findColByRowIds, vector is null" << endl;
                    cout << endl;
                    return;
                }
                for (uint64_t k=0; k < val->size(); k++){
                    sum += (*val)[k];
                }
                delete(val);
                if (i == this->parser.selections.size() - 1)
                {
                    cout << sum;
                }
                else
                {
                    cout << sum << " ";
                }
            }
            else{
                cerr << "No relations in mid result to print" << endl;
                cout << endl;
                return;
            }
        }
        cout << endl;
        return;
    }
    else{
        cout << endl;
        return;
    }
}



void Query::computeFilters(FileArray & fileArray){

    //filters first
    for (int i=0; i<this->parser.filters.size(); i++){
        int index=-1;
        
        for (int j=0; j<this->midResults.size(); j++){
            if (this->midResults[j].indexs != NULL){
                if ( (*(this->midResults[j].indexs)).front() == this->parser.filters[i].index){
                    index=j;
                    break;    
                }
            }
        }
        if (index < 0){
            RelationResults res ;
            res.relPos = this->parser.filters[i].rel;
            res.rowIds = new vector<uint64_t>;
            res.index = this->parser.filters[i].index;
            for (uint64_t k=0; k < fileArray.getRowNum(res.relPos); k++){
                res.rowIds->push_back(k);
            }
            MidResult result;
            result.res = new vector<RelationResults>;
            result.res->push_back(res);
            result.rels = new vector<int>;
            result.rels->push_back(res.relPos);
            result.indexs = new vector<int>;
            result.indexs->push_back(res.index);
            this->midResults.push_back(result);
            index = midResults.size()-1;        
        }
        
        //CHECK IF MIDRESULT EMPTY
        if ( this->midResults[index].res->front().rowIds->size() == 0){
            // do nothing
            continue;
        }
        if (this->parser.filters[i].op == FilterInfo::FilterOperation::LESS){
            //NONE
            if(fileArray.getColMin(this->parser.filters[i].rel, this->parser.filters[i].col) >= this->parser.filters[i].constant){
                this->midResults[index].res->front().rowIds->clear();   //empty vector (constant complexity)
                continue;
            }
            //ALL
            if(fileArray.getColMax(this->parser.filters[i].rel, this->parser.filters[i].col) < this->parser.filters[i].constant){
                //do nothing
                continue;
            }
        }
        if (this->parser.filters[i].op == FilterInfo::FilterOperation::GREATER){
            //NONE
            if(fileArray.getColMax(this->parser.filters[i].rel, this->parser.filters[i].col) <= this->parser.filters[i].constant){
                this->midResults[index].res->front().rowIds->clear();   //empty vector (constant complexity)
                continue;
            }
            //ALL
            if(fileArray.getColMin(this->parser.filters[i].rel, this->parser.filters[i].col) > this->parser.filters[i].constant){
                //do nothing
                continue;
            }
        }
        this->filters.filter( this->midResults[index].res->front(), this->parser.filters[i], fileArray);
     
    }
    
}

void Query::computeJoins(FileArray &fileArray){
 
    // computer already sorted joins
    for ( int joinIndex = 0; joinIndex < this->parser.joins.size(); joinIndex++ ) {
        // get rels to be joined
        int leftRel = this->parser.joins[joinIndex].rel1;
        int rightRel = this->parser.joins[joinIndex].rel2;
        int leftIndex = this->parser.joins[joinIndex].index1;
        int rightIndex = this->parser.joins[joinIndex].index2;
        // find midResult index for each rel
        int index1 = -1, index2 = -1;
        for ( int midIndex = 0; midIndex < midResults.size(); midIndex++ ) {
            // look for rels inside current midResults
            for ( int relIndex = 0; relIndex < midResults[midIndex].indexs->size(); relIndex++ ) {
                if ( leftIndex == midResults[midIndex].indexs->at(relIndex) ) {
                    index1 = midIndex;
                }
                if ( rightIndex == midResults[midIndex].indexs->at(relIndex) ) {
                    index2 = midIndex;
                }
            }
        }
        // if rel1 wasn't found, create midResult
        if ( index1 == -1 ) {
            // Create temporary result
            RelationResults res ;
            res.relPos = leftRel;
            res.index = leftIndex;
            res.rowIds = new vector<uint64_t>;
            for (uint64_t k=0; k < fileArray.getRowNum(res.relPos); k++){
                res.rowIds->push_back(k);
            }
            // Create temp midResult
            MidResult tempMid;
            tempMid.res = new vector<RelationResults>;
            tempMid.res->push_back(res);
            tempMid.rels = new vector<int>;  
            tempMid.rels->push_back(leftRel);
            tempMid.indexs = new vector<int>;  
            tempMid.indexs->push_back(leftIndex);
            // Push new mid result into mid results
            midResults.push_back(tempMid);
            index1 = midResults.size() - 1;
        }
        // if rel2 wasn't found, create midResult
        if ( index2 == -1 ) {
            // Create temporary result
            RelationResults res ;
            res.relPos = rightRel;
            res.index = rightIndex;
            res.rowIds = new vector<uint64_t>;
            for (uint64_t k=0; k < fileArray.getRowNum(res.relPos); k++){
                res.rowIds->push_back(k);
            }
            // Create temp midResult
            MidResult tempMid;
            tempMid.res = new vector<RelationResults>;
            tempMid.res->push_back(res);
            tempMid.rels = new vector<int>;  
            tempMid.rels->push_back(rightRel);
            tempMid.indexs = new vector<int>;  
            tempMid.indexs->push_back(rightIndex);
            // Push new mid result into mid results
            midResults.push_back(tempMid);
            index2 = midResults.size() - 1;
        }
        // If our relations are on the same MidResult
        if ( index1 == index2 ) {
            // SAME REL JOIN
            this->joins.join(midResults[index1], this->parser.joins[joinIndex],  fileArray );
            continue;
        }
        // In any other case, use Radix Hash Join
        MidResult * FinalResults =this->joins.join(midResults[index1],midResults[index2],this->parser.joins[joinIndex],fileArray);
        // Erase previous mid results and push the new one
        if ( index1 < index2 ) {
            midResults.erase(midResults.begin()+index2);
            midResults.erase(midResults.begin()+index1);
        } else {
            midResults.erase(midResults.begin()+index1);
            midResults.erase(midResults.begin()+index2);
        }
        midResults.push_back( *FinalResults );
        
    }

}

void Query::computeSelfJoins(FileArray &fileArray){
    for ( int SelfIndex = 0; SelfIndex < this->parser.selfs.size(); SelfIndex++ ) {
        // get rels to be joined
        int rel = this->parser.selfs[SelfIndex].rel;
        int index = this->parser.selfs[SelfIndex].index;
        // find midResult index for each rel
        int index1 = -1;
        for ( int midIndex = 0; midIndex < midResults.size(); midIndex++ ) {
            // look for rels inside current midResults
            for ( int relIndex = 0; relIndex < midResults[midIndex].indexs->size(); relIndex++ ) {
                if ( index == midResults[midIndex].indexs->at(relIndex) ) {
                    index1 = midIndex;
                }
            }
        }
        // if rel1 wasn't found, create midResult
        if ( index1 == -1 ) {
            // Create temporary result
            RelationResults res ;
            res.relPos = rel;
            res.index = index;
            res.rowIds = new vector<uint64_t>;
            for (uint64_t k=0; k < fileArray.getRowNum(res.relPos); k++){
                res.rowIds->push_back(k);
            }
            // Create temp midResult
            MidResult tempMid;
            tempMid.res = new vector<RelationResults>;
            tempMid.res->push_back(res);
            tempMid.rels = new vector<int>;  
            tempMid.rels->push_back(rel);
            tempMid.indexs = new vector<int>;  
            tempMid.indexs->push_back(index);
            // Push new mid result into mid results
            midResults.push_back(tempMid);
            index1 = midResults.size() - 1;
        }
        
        // SELF JOIN
        JoinInfo tempJoin;
        tempJoin.rel1 = rel;
        tempJoin.rel2 = rel;
        tempJoin.index1 = index;
        tempJoin.index2 = index;
        tempJoin.col1 = this->parser.selfs[SelfIndex].col1;
        tempJoin.col2 = this->parser.selfs[SelfIndex].col2;
        this->joins.join(midResults[index1], tempJoin, fileArray );
    }    
}

void Query::computeQuery(FileArray & fileArray, string & line) {
    //parse Query info
    this->parser.parseQuery(line);
    //rearange computations (optimize)
    this->optimize.optimizeQuery(fileArray, this->parser);
    //do computations
    //compute Filters first
    this->computeFilters(fileArray);
    //compute Joins after
    this->computeSelfJoins(fileArray);
    this->computeJoins(fileArray);
    //print results to std::out
    this->printResult(fileArray);
}
