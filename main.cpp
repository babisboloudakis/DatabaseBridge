#include <iostream>
#include "headers/query.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    vector <string> fileNames;
    string line;
    
    while (getline(cin, line)){
        if (line == "Done") break;
        fileNames.push_back(line);
    }
    for (int i = 0; i< fileNames.size(); i++){
        cout << fileNames[i] << endl;
    }
    cout << endl;

    FileArray fileArray(fileNames, fileNames.size());

    for (int i=0; i < fileArray.getSize(); i++){
        if(fileArray.computeStatistics(i)){
            cerr << "Error in computeStatitics in " << i << " file" << endl;
        }
        // for (int j=0; j < fileArray.getColNum(i); j++){
        //     vector<uint64_t> r;
        //     for (uint64_t k=0; k<fileArray.getRowNum(i); k++){
        //         r.push_back(k);
        //     }
        //     vector<uint64_t> * v = fileArray.findColByRowIds(r, j, i);
        //     if(v == NULL){
        //         cout << "Error in findcol" << endl;
        //         continue;
        //     }
        //     cout << "column has first 3 values : "<< (*v)[0] << ", " << (*v)[1] << ", " << (*v)[2] << endl << endl;
        //     cout << j << " column has statistics: ";
        //     cout << fileArray.getColMax(i,j) << " max, " ;
        //     cout << fileArray.getColMin(i,j) << " min, " ;
        //     cout << fileArray.getColUniqueNum(i,j) << " uniqueNum" << endl;
        // }
        // cout << endl;
    }
    
    while (getline(cin, line)) {
        if (line == "F") continue; // End of a batch
        if (line == "Done") break; // End of queries
        // Create new parser to hold query info
        Query query;
        query.computeQuery(fileArray, line);
    }

    return 0;
    
}
