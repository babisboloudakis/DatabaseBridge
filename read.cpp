#include <iostream>
#include <string>

using namespace std;

class Cell {
    private:
    char ** array;
    public:
    string fileName;
    uint64_t colNum;
    uint64_t rowNum;

    Cell(string name, uint64_t col, uint64_t row): fileName(name), colNum(col), rowNum(row){
        this->array = new char*[col];
        for (int i=0; i < col; i++){
            this->array[i] = null;
        }
    }

    void storeRow(char * addr, int col){
        this->array[col-1] = addr;
        return;
    }

    int findStatistics(){
        if (this->array[0] == null){
            return 1; //error
        }
        //min, max value
        //unique values(count, or items)
        //size (already in colNum, rowNum)
        return 0;
    }
};

class FileArray {
    private:
    Cell * cells;
    public:
    int size;

    FileArray(string * fileName, int s): size(s){
        this->cells = new Cell[size];
        for (int i=0; i<size; i++){
            
            //relation.cpp code
            //get pointer
            int fd = open(fileName[i], O_RDONLY);
            if (fd==-1) {
                cerr << "cannot open " << fileName[i] << endl;
                throw;
            }

            // Obtain file size
            struct stat sb;
            if (fstat(fd,&sb)==-1)
                cerr << "fstat\n";

            int length=sb.st_size;

            char* addr=static_cast<char*>(mmap(nullptr,length,PROT_READ,MAP_PRIVATE,fd,0u));
            
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
            uint64_t col = *addr;
            addr += sizeof(col);
            this.cells[i] = new Cell(fileName[i], col, row);
            for (int j=0; j<col; j++){ 
                if (this->cells[i]->storeRow(addr, col) == 1){
                    cerr << "error in storeRow in" << filename[i] << endl;
                    throw;
                }
                addr += row*sizeof(uint64_t);         
            }
            
        }
        
    }       
};


int main(void){
    string filename;
    cin >> filename ;
    cout << filename ;

}