#include <iostream>
#include "parser.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    vector <string> fileNames;
    string line;
    // cin >> line;
    // while (line != "Done"){
    //     fileNames.push_back(line);
    //     cin >> line;
    // }
    while (getline(cin, line)){
        if (line == "Done") break;
        fileNames.push_back(line);
    }
    cout << "Printing ...." << endl;
    for (int i = 0; i< fileNames.size(); i++){
        cout << fileNames[i] << endl;
    }
    cout << endl;

    FileArray fileArray(fileNames, fileNames.size());
    
    // cout << "fileArray size: "<< fileArray.getSize() << endl ;

    for (int i=0; i < fileArray.getSize(); i++){
        cout << "File: " << i << endl;
        if(fileArray.computeStatistics(i)){
            cerr << "Error in computeStatitics in " << i << " file" << endl;
        }
        for (int j=0; j < fileArray.getColNum(i); j++){
            vector<uint64_t> r;
            for (uint64_t k=0; k<fileArray.getRowNum(i); k++){
                r.push_back(k);
            }
            vector<uint64_t> * v = fileArray.findColByRowIds(r, j, i);
            if(v == NULL){
                cout << "Error in findcol" << endl;
                continue;
            }
            cout << "column has first 3 values : "<< (*v)[0] << ", " << (*v)[1] << ", " << (*v)[2] << endl << endl;
            
            cout << j << " column has statistics: ";
            cout << fileArray.getColMax(i,j) << " max, " ;
            cout << fileArray.getColMin(i,j) << " min, " ;
            cout << fileArray.getColUniqueNum(i,j) << " uniqueNum" << endl;
        }
        cout << endl;
    }
    cout << "Done computing for all files" << endl;
    
    Parser parser;
    cout << "Insert Query..." << endl;
    while (getline(cin, line)) {
        if (line == "F") continue; // End of a batch
        if (line == "Done") break;
        // cout << "Just Got line : " << line << endl;
        // parser.parseQuery(line);
        // parser.printParseInfo();
        // 0|0.1>5000|0.0 0.1 0.2
        parser.computeQuery(fileArray, line);
        
    }
    cout << "Exiting..." << endl;
    return 0;
}
