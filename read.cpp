#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <vector>



using namespace std;

class Cell {
    public:
    char ** array;
    string fileName;
    uint64_t colNum;
    uint64_t rowNum;

    Cell(string name, uint64_t col, uint64_t row): fileName(name), colNum(col), rowNum(row){
        this->array = new char*[col];
        for (int i=0; i < col; i++){
            this->array[i] = NULL;
        }
    }

    void storeRow(char * addr, int col){
        this->array[col-1] = addr;
        return;
    }

    int findStatistics(){
        if (this->array[0] == NULL){
            return 1; //error
        }
        //min, max value
        //unique values(count, or items)
        //size (already in colNum, rowNum)
        return 0;
    }
};

class FileArray {
    public:
    Cell** cells;
    int size;

    FileArray(vector<string> &fileName, int s): size(s){
        this->cells = new Cell * [s];
        
        for (int i=0; i<size; i++){
            
            //relation.cpp code
            //get pointer
            int fd = open(fileName[i].c_str(), O_RDONLY);
            if (fd==-1) {
                cerr << "cannot open " << fileName[i] << endl;
                throw;
            }

            // Obtain file size
            struct stat sb;
            if (fstat(fd,&sb)==-1)
                cerr << "fstat\n";

            int length=sb.st_size;

            char* addr=static_cast<char*>(mmap(NULL,length,PROT_READ,MAP_PRIVATE,fd,0u));
            
            if (addr==MAP_FAILED) {
                cerr << "cannot mmap " << fileName[i] << " of length " << length << endl;
                throw;
            }

            if (length<16) {
                cerr << "relation file " << fileName[i] << " does not contain a valid header" << endl;
                throw;
            }

            //relation.cpp code

            uint64_t row = *addr;
            addr += sizeof(row);
            cout << "RowNum: " << row << endl;
            
            uint64_t col = *addr;
            cout << "ColNum: " << col << endl;
            addr += sizeof(col);
            this->cells[i] = new Cell(fileName[i], col, row);
            for (int j=0; j<col; j++){ 
                this->cells[i]->storeRow(addr, col);
                uint64_t data = *addr;
                cout << j << " col first data is: " << data << endl;
                addr += row*sizeof(uint64_t);         
            }

        }

            
    }
       
};


int main(void){
    
    // vector <char *> fileNames;
    vector <string> fileNames;

    string word;
    cin >> word ;
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
    
    cout << fileArray.cells[0]->colNum << endl;
    
    cout << endl << "Exit" << endl;


}