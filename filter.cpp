#include "filter.hpp"
#include <iostream>
#include "parser.hpp"
#include <vector>

using namespace std;

void filterResults( RelationResults & results, FilterInfo filter, FileArray & fileArray ) {

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

int main ( void ) {

    string str = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1";
    cout << str << endl;
    Parser parser;
    parser.parseQuery(str);
    parser.printParseInfo();
}