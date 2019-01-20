#include <iostream>
#include "headers/query.hpp"

extern JobScheduler scheduler;

using namespace std;

int main(int argc, char* argv[]) {

    vector <string> fileNames;
    string line;
    while (getline(cin, line)){
        if (line == "Done") break;
        fileNames.push_back(line);
    }

    FileArray fileArray(fileNames, fileNames.size());

    for (int i=0; i < fileArray.getSize(); i++){
        if(fileArray.computeStatistics(i)){
            cerr << "Error in computeStatitics in " << i << " file" << endl;
        }
    }

    // Create Threds.
    scheduler.init();
    
    while (getline(cin, line)) {
        if (line == "F") continue; // End of a batch
        if (line == "Done") break; // End of queries
        // Create new parser to hold query info
        Query query;
        query.computeQuery(fileArray, line);
    }

    // Destroy threads and Job Scheduler Data structures.
    scheduler.destroy();
    
    return 0;
    
}
