#include <iostream>
#include "headers/filter.hpp"
#include <vector>
#include <cstdint>
#include <cstring>

using namespace std;

void Filter::filter( RelationResults & results, FilterInfo filter, FileArray & fileArray ) {

    // New vector with results after filtering
    vector<uint64_t> * filteredResults = new vector<uint64_t>;

    // Get the values to be filtered throught the mid results
    vector<uint64_t> * values = fileArray.findColByRowIds( *(results.rowIds), filter.col, filter.rel );

    // Loop throught results and apply filter
    for ( int index = 0; index < values->size(); index++ ) {

        // Execute selected filter operation
        switch( filter.op ) {
            case FilterInfo::FilterOperation::LESS:
                if ( (*values)[index] < filter.constant ) { 
                    filteredResults->push_back( (*results.rowIds)[index] );
                }
                break;
            case FilterInfo::FilterOperation::GREATER:
                if ( (*values)[index] > filter.constant ) {
                    filteredResults->push_back( (*results.rowIds)[index] );
                }
                break;
            case FilterInfo::FilterOperation::EQUAL:
                if ( (*values)[index] == filter.constant ) {
                    filteredResults->push_back( (*results.rowIds)[index] );
                }
                break;
        }

    }

    // delete previous unnecessary vector
    delete results.rowIds;
    results.rowIds = filteredResults;
    // Delete values allocated by findColByRowIds
    delete values;
}

void Filter::filter( MidResult & results, FilterInfo & filter, FileArray & fileArray){
    int index;
    vector<uint64_t> * values;
    vector<RelationResults> * newRes = new vector<RelationResults>;

    for (int i=0; i < results.rels->size(); i++){
        if (results.rels->at(i) == filter.rel){
            index = i;
        }
        RelationResults temp;
        temp.relPos = results.rels->at(i);
        temp.rowIds = new vector<uint64_t>;
        newRes->push_back(temp);
    }     

    values = fileArray.findColByRowIds( *(results.res->at(index).rowIds), filter.col, filter.rel );
    
    // Execute selected filter operation
    switch( filter.op ) {
        case FilterInfo::FilterOperation::LESS:
            // Loop throught results and apply filter
            for ( int i = 0; i < values->size(); i++ ) {
                if ( (*values)[index] < filter.constant ) { 
                    for (int j=0; j<results.rels->size(); j++){
                        newRes->at(j).rowIds->push_back(results.res->at(j).rowIds->at(i));
                    }
                }
            }
            break;
        case FilterInfo::FilterOperation::GREATER:
            // Loop throught results and apply filter
            for ( int i = 0; i < values->size(); i++ ) {
                if ( (*values)[index] > filter.constant ) {
                    for (int j=0; j<results.rels->size(); j++){
                        newRes->at(j).rowIds->push_back(results.res->at(j).rowIds->at(i));
                    }
                }
            }
            break;
        case FilterInfo::FilterOperation::EQUAL:
            // Loop throught results and apply filter
            for ( int i = 0; i < values->size(); i++ ) {
                if ( (*values)[index] == filter.constant ) {
                    for (int j=0; j<results.rels->size(); j++){
                        newRes->at(j).rowIds->push_back(results.res->at(j).rowIds->at(i));
                    }
                }
            }
            break;
    }
    for (int j=0; j<results.rels->size(); j++){
        delete (results.res->at(j).rowIds);
    }
    delete (results.res);
    results.res = newRes;
    
    return;   
}

