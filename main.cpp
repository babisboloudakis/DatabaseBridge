#include <iostream>
// #include "Joiner.hpp"
#include "parser.hpp"
#include "read.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    vector <string> fileNames;
    string word;
    cin >> word;
    while (word != "Done"){
        fileNames.push_back(word);
        cin >> word;
    }
    cout << "Printing ...." << endl << endl;
    for (int i = 0; i< fileNames.size(); i++){
        cout << fileNames[i] << endl;
    }
    cout << endl;

    FileArray fileArray(fileNames, fileNames.size());
    
    cout << "fileArray size: "<< fileArray.getSize() << endl ;

    for (int i=0; i < fileArray.getSize(); i++){
        // cout << "File: " << i << endl;
        // if(fileArray.computeStatistics(i)){
        //     cerr << "Error in computeStatitics in " << i << " file" << endl;
        // }
        for (int j=0; j < fileArray.getColNum(i); j++){
            cout << j << " column has statistics: ";
            cout << fileArray.getColMax(i,j) << " max, " ;
            cout << fileArray.getColMin(i,j) << " min, " ;
            cout << fileArray.getColUniqueNum(i,j) << " uniqueNum" << endl;
        }
        cout << endl;
    }
    
    
    cout << endl << "Exit" << endl;
    
    // Parser i;
    // while (getline(cin, line)) {
    //     if (line == "F") continue; // End of a batch
    //     i.parseQuery(line);
    //     cout << joiner.join(i);
    // }
    // return 0;
}
