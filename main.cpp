#include <iostream>
#include "parser.hpp"

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
    cout << "fileArray size: "<< fileArray.getSize() << endl ;

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
            
        }
        cout << endl;
    }
    
    while (getline(cin, line)) {
        if (line == "F") continue; // End of a batch
        if (line == "Done") break;
        // cout << "Just Got line : " << line << endl;
        // parser.parseQuery(line);
        // parser.printParseInfo();
        // 0 1|1.2=0.2|0.1 1.1
        Parser parser;
        parser.computeQuery(fileArray, line);
        
    }
    cout << "Exiting..." << endl;
    return 0;
}
