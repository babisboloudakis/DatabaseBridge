#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <vector>
#include <unordered_set>

using namespace std;

typedef struct ColInfo {
    uint64_t * pointer;
    uint64_t min;
    uint64_t max;
    uint64_t uniqueNum;
}colInfo;

class Cell {
    private:
    colInfo * array;
    string fileName;
    uint64_t colNum;
    uint64_t rowNum;
    public:
    

    Cell(string name, uint64_t col, uint64_t row): fileName(name), colNum(col), rowNum(row){
        this->array = new colInfo[col];
        for (int i=0; i < col; i++){
            this->array[i].pointer = NULL;
            this->array[i].min = 0;
            this->array[i].max = 0;
            this->array[i].uniqueNum = 0;
        }
    };

    ~Cell(){
        // for (int i=0; i < this->colNum; i++){
        //     this->array[i].pointer  dont know what to do yet  
        // }
        delete(this->array);
    };

    void storeRow(uint64_t * addr, int col);

    int computeStatistics();

    vector<uint64_t> * findColByRowIds (vector<uint64_t> &rowId, uint64_t col);

    uint64_t * getColPtr (uint64_t col);

    uint64_t getColMax (uint64_t col);

    uint64_t getColMin(uint64_t col);

    uint64_t getColUniqueNum(uint64_t col);

    uint64_t getRowNum();

    uint64_t getColNum();

    string getFileName();

    colInfo * getArray();
    
};

class FileArray {
    private:
    Cell** cells;
    int size;
    public:
    

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

            uint64_t row = *(uint64_t *)addr;
            addr += sizeof(row);
            cout << "RowNum: " << row << endl;
            
            uint64_t col = *(uint64_t *)addr;
            cout << "ColNum: " << col << endl;
            addr += sizeof(col);
            this->cells[i] = new Cell(fileName[i], col, row);
            
            // string str[3];
            // str[0] = "./f0.txt";
            // str[1] = "./f1.txt";
            // str[2] = "./f2.txt";

            for (int j=0; j<col; j++){
                this->cells[i]->storeRow((uint64_t *)addr, j);
                // ofstream myfile;
                // myfile.open (str[j]);
                // for(int k=0; k< row; k++){
                //     myfile << *(uint64_t *)(addr + k*sizeof(uint64_t)) << endl;
                // }
                uint64_t data = *(uint64_t *)(addr + sizeof(uint64_t));
                cout << j << " col second data is: " << data << endl;
                addr += row*sizeof(uint64_t);        
            }
            this->computeStatistics(i);
        }

    };

    ~FileArray(){
        for (int i=0; i < this->size; i++){
            delete(cells[i]);
        }
    };

    vector<uint64_t> *findColByRowIds(vector<uint64_t> &rowId, uint64_t col, int relPos);

    uint64_t getRowNum(int relPos);

    uint64_t getColNum(int relPos);

    uint64_t * getColPtr (int relPos, uint64_t col);

    string getFileName(int relPos);
    
    uint64_t getColMax (int relPos, uint64_t col);

    uint64_t getColMin(int relPos, uint64_t col);

    uint64_t getColUniqueNum(int relPos, uint64_t col);

    int getSize();

    int computeStatistics(int relPos);
 
};