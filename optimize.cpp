#include "optimize.hpp"

using namespace std;

void Optimize::optimizeQuery(FileArray & fileArray, Parser & parser){
    //optimize Filters
    int score; //min?
    int index;
    for(int i=0; i<parser.joins.size(); i++){
        uint64_t min1 = fileArray.getColMin(parser.joins[i].rel1, parser.joins[i].col1);
        uint64_t max1 = fileArray.getColMax(parser.joins[i].rel1, parser.joins[i].col1);
        uint64_t min2 = fileArray.getColMin(parser.joins[i].rel2, parser.joins[i].col2);
        uint64_t max2 = fileArray.getColMax(parser.joins[i].rel2, parser.joins[i].col2);
        uint64_t uniqueNum1 = fileArray.getColUniqueNum(parser.joins[i].rel1, parser.joins[i].col1);
        uint64_t uniqueNum2 = fileArray.getColUniqueNum(parser.joins[i].rel2, parser.joins[i].col2);
        uint64_t size1 = fileArray.getRowNum(parser.joins[i].rel1);
        uint64_t size2 = fileArray.getRowNum(parser.joins[i].rel2);
        int temp;
        //select min = size1 + size2
        if (i == 0){
            score = size1 + size2;
            index = 0;
        }
        else{
            temp = size1 + size2;
            if  (temp < score){
                score = temp;
                index = i;
            }
        }
        //MIN 1 > MAX 2 || MIN 2 > MAX 1 => NULL
        if (min1 > max2 || min2 > max1){
            score = 0;
            index = i;
        }
        
    }
    JoinInfo join = parser.joins.at(index);
    parser.joins.erase(parser.joins.begin() + index);
    parser.joins.insert(parser.joins.begin(), join);
    
    return;
}