#include <iostream>

using namespace std;

typedef struct midResult {
    // struct for midResult vector
        

} midResult;


vector<midResult> * filterResults ( vector<midResult> * results, uint64_t rel, uint64_t col, filterOperation op , uint64_t value ) {

    // New vector with results after filtering
    vector<midResult> * filteredResults;

    // Get the values to be filtered throught the mid results
    vector<uint64_t> * values = findColByRowIds( results );

    // Loop throught results and apply filter
    for ( int index = 0; index < values.size(); index++ ) {

        // Execute selected filter operation
        switch( op ) {
            case LESS:
                if ( values[index] < value ) {
                    filteredResults.push_back( results[index] );
                }
                break;
            case GREATER:
                if ( values[index] > value ) {
                    filteredResults.push_back( results[index] );
                }
                break;
            case EQUAL:
                if ( values[index] == value ) {
                    filteredResults.push_back( results[index] );
                }
                break;
        }

    }

    // delete previous unnecessary vector
    delete results;
    delete values;

    // return results
    return filteredResults;

}
