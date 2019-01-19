#include "optimize.hpp"
#include "filter.hpp"
#include "scheduler.hpp"

using namespace std;
 
class Query {
    private:
    //midreasult
    vector<MidResult> midResults;
    //parser
    Parser parser;
    //optimizer
    Optimize optimize;
    //filters
    Filter filters;
    //joins
    Join joins;

    public:
    Query(){}; 
    ~Query(){
        for (int i=0; i < this->midResults.size(); i++){
            if (this->midResults[i].res != NULL){
                for (int j=0; j < this->midResults[i].res->size(); j++){
                    if ( (*(this->midResults[i].res))[j].rowIds != NULL){
                        delete ((*(this->midResults[i].res))[j].rowIds);    
                    }
                }
                delete(this->midResults[i].res);
            }
            if (this->midResults[i].rels != NULL){
                delete(this->midResults[i].rels); 
            }
        }    
    }; //delete pointers

    void computeQuery(FileArray & fileArray, string & line);
    private:
    //fnct -> prakseis((min, max)fliters->midresults epilogh twn joins->joins diaxeirish mid result)
    void computeFilters(FileArray & fileArray);
    void computeJoins(FileArray & fileArray);
    void printResult(FileArray & fileArray);
    //applyfiter(&RelationResults, col, &fileArray ) (vector[2],f)
    //applyjoin to diff()
    //apply join to same()
};