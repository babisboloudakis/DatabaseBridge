#include "read.hpp"

using namespace std;

//CELL

void Cell::storeRow(uint64_t * addr, int col){ //col starts counting from 0
    this->array[col].pointer = addr;
    return;
}

int Cell::computeStatistics(){        //returns 1 if error
    if (this->array[0].pointer == NULL){
        return 1; //error
    }
    for (int i=0; i < (this->colNum); i++){
        unordered_set<uint64_t> set;
        uint64_t temp;
        uint64_t min;
        uint64_t max;
        uint64_t * ptr = array[i].pointer;
        for (int j=0; j < (this->rowNum); j++){
            
            temp = *(uint64_t *)(ptr);
            //UNIQUE NUM
            if (set.find(temp) == set.end()){   //if not in set
                set.insert(temp);               //add it
            }
            //MIN
            if (j == 0){
                min = temp;
            }
            if (temp < min){ 
                min = temp;
            }
            //MAX
            if (j == 0){ 
                max = temp;
            }
            if (temp > max){ 
                max = temp;
            }
            ptr += 1;    
        }
        this->array[i].min = min;
        this->array[i].max = max;
        this->array[i].uniqueNum = set.size();
    }
    return 0;
}

vector<uint64_t> * Cell::findColByRowIds (vector<uint64_t> &rowId, uint64_t col){
    if (rowId.size() == 0){
        return NULL;
    }
    vector<uint64_t> *p = new vector<uint64_t>;
    uint64_t * addr = this->array[col].pointer;
    for (int i=0; i<rowId.size(); i++){
        p->push_back(*(addr + rowId[i]));
    }
    return p;
}



uint64_t * Cell::getColPtr (uint64_t col){
    return this->array[col].pointer;
}

uint64_t Cell::getColMax (uint64_t col){
    return this->array[col].max;
}

uint64_t Cell::getColMin(uint64_t col){
    return this->array[col].min;
}

uint64_t Cell::getColUniqueNum(uint64_t col){
    return this->array[col].uniqueNum;
}

uint64_t Cell::getRowNum(){
    return this->rowNum;
}

uint64_t Cell::getColNum(){
    return this->colNum;
}

string Cell::getFileName(){
    return this->fileName;
}

colInfo * Cell::getArray(){
    return this->array;
}

//FILEARRAY

vector<uint64_t> * FileArray::findColByRowIds(vector<uint64_t> &rowId, uint64_t col, int relPos){
    if(relPos >= size){
        return NULL;
    }
    return this->cells[relPos]->findColByRowIds(rowId, col);
}

uint64_t FileArray::getRowNum(int relPos){
    return this->cells[relPos]->getRowNum();
}

uint64_t FileArray::getColNum(int relPos){
    return this->cells[relPos]->getColNum();
}

uint64_t * FileArray::getColPtr (int relPos, uint64_t col){
    return this->cells[relPos]->getColPtr(col);
}

string FileArray::getFileName(int relPos){
    return this->cells[relPos]->getFileName();
}

uint64_t FileArray::getColMax (int relPos, uint64_t col){
    return this->cells[relPos]->getColMax(col);
}

uint64_t FileArray::getColMin(int relPos, uint64_t col){
    return this->cells[relPos]->getColMin(col);
}

uint64_t FileArray::getColUniqueNum(int relPos, uint64_t col){
    return this->cells[relPos]->getColUniqueNum(col);
}

int FileArray::getSize(){
    return this->size;
}

int FileArray::computeStatistics(int relPos){
    int flag = 0;
    for (int i=0; i < this->cells[relPos]->getColNum(); i++){
        if(this->cells[relPos]->computeStatistics()){
            flag = 1;
        }       
    }
}