#include "parser.hpp"
#include <sstream>

using namespace std;

static void splitString(string& line, vector<string>& result, const char delim) {
    stringstream ss(line);
    string token;
    while ( getline(ss,token,delim) ) {
        result.push_back(token);
    }
}

static void splitPredicates(string& line, vector<string>& result) {
  // Split a line into predicate strings
  // Determine predicate type
  for (auto cT : comparisonTypes ) {
    if (line.find(cT)!=string::npos) {
      splitString(line,result,cT);
      break;
    }
  }
}

void Parser::printResult(FileArray &fileArray){
    for(int i=0; i<this->selections.size(); i++){
        int index=-1;
        vector<uint64_t> * val;
        uint64_t sum=0;
        for (int j=0; j<this->results.size(); j++){
            if (this->results[j].relPos == this->selections[i].rel){
                index=j;
            }
        }
        if (index < 0){
            RelationResults res;
            res.relPos = this->selections[i].rel;
            res.rowIds = new vector<uint64_t>;
            for (uint64_t k=0; k < fileArray.getRowNum(this->selections[i].rel); k++){
                res.rowIds->push_back(k);
            }
            this->results.push_back(res);
            index = results.size()-1;        
        }            
        if (this->results[index].rowIds->size() == 0){
            cout << selections[i].rel << " : " << "NULL" << endl;
            continue;
        }
        val = fileArray.findColByRowIds(*(this->results[index].rowIds), this->selections[i].col, this->results[index].relPos);
        if (val == NULL){
            cerr << "Error in findColByRowIds, vector is null" << endl;
        }
        
        for (uint64_t k=0; k < val->size(); k++){
            sum += (*val)[k];
        }
        delete(val);
        cout << "rel: " << selections[i].rel << " ,col: " << this->selections[i].col << " ,sum: " << sum << endl;     
    }
}

void Parser::compute(FileArray & fileArray){
    //filters first
    
    for (int i=0; i<this->filters.size(); i++){
        int index=-1;
        for (int j=0; j<this->results.size(); j++){
            if (this->results[j].relPos == this->filters[i].rel){
                index=j;
            }
        }
        if (index < 0){
            RelationResults res;
            res.relPos = this->filters[i].rel;
            res.rowIds = new vector<uint64_t>;
            for (uint64_t k=0; k < fileArray.getRowNum(res.relPos); k++){
                res.rowIds->push_back(k);
            }
            this->results.push_back(res);
            index = results.size()-1;        
        }
        
        //CHECK IF MIDRESULT EMPTY
        if ( this->results[index].rowIds->size() == 0){
            // do nothing
            continue;
        }

        if (this->filters[i].op == FilterInfo::FilterOperation::LESS){
            //NONE
            if(fileArray.getColMin(this->filters[i].rel, this->filters[i].col) >= this->filters[i].constant){
                this->results[index].rowIds->clear();   //empty vector (constant complexity)
                continue;
            }
            //ALL
            if(fileArray.getColMax(this->filters[i].rel, this->filters[i].col) < this->filters[i].constant){
                //do nothing
                continue;
            }
        }
        if (this->filters[i].op == FilterInfo::FilterOperation::GREATER){
            //NONE
            if(fileArray.getColMax(this->filters[i].rel, this->filters[i].col) <= this->filters[i].constant){
                this->results[index].rowIds->clear();   //empty vector (constant complexity)
                continue;
            }
            //ALL
            if(fileArray.getColMin(this->filters[i].rel, this->filters[i].col) > this->filters[i].constant){
                //do nothing
                continue;
            }
        }
        filterResults( this->results[index], this->filters[i], fileArray);
     
    }
    

}

void Parser::optimize(FileArray & fileArray){
    //do nothing for now
    return;
}

void Parser::computeQuery(FileArray & fileArray, string & line) {
    //parse Query info
    this->parseQuery(line);
    //debug
    this->printParseInfo();
    //rearange computations (optimize)
    this->optimize(fileArray);
    //do computations
    this->compute(fileArray);
    //print results to std::out
    this->printResult(fileArray);
}

void Parser::parseRelCol ( string & rawPair ) { 
    // Parse rel col pair
    vector<string> words;
    // Split string into words
    splitString(rawPair,words,'.');
    selections.emplace_back( SelectInfo( relations[stoul(words[0])] , stoul(words[1]) ) );
}

void Parser::parseRelations( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,' ');
    for ( int i = 0; i < words.size(); i++ ) {
        relations.push_back( stoul( words[i] ) );
    }
}

SelectInfo Parser::parsePair ( string & rawPair ) { 
    // Parse rel col pair
    vector<string> words;
    // Split string into words
    splitString(rawPair,words,'.');
    return SelectInfo( stoul(words[0]) , stoul(words[1]) );
}

inline static bool isConstant(string& raw) { return raw.find('.')==string::npos; }

void Parser::parsePredicate(string& rawPredicate) {

    vector<string> preds;
    splitPredicates(rawPredicate, preds);
    SelectInfo left = parsePair(preds[0]);
    if ( isConstant(preds[1]) ) { // filter operation
        char compType=rawPredicate[preds[0].size()];
        filters.emplace_back( FilterInfo( relations[left.rel], left.col, stoul(preds[1]), FilterInfo::FilterOperation(compType) ) );
    } else { // Join operation
        SelectInfo right = parsePair(preds[1]);
        joins.emplace_back( JoinInfo( relations[left.rel], left.col, relations[right.rel], right.col ) );
    }

}

void Parser::parsePredicates( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,'&');
    for ( int i = 0; i < words.size(); i++ ) {
        parsePredicate(words[i]);
    }
}

void Parser::parseProjections( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,' ');
    for ( int i = 0; i < words.size(); i++ ) {
        parseRelCol(words[i]);
    }
}

void Parser::parseQuery( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,'|');
    
    parseRelations(words[0]);
    parsePredicates(words[1]);
    parseProjections(words[2]);
 
}

void Parser::printParseInfo() {
    // Print parse info for debugging purposes
    cout << "Relations" << endl;
    for ( int i = 0; i < relations.size(); i++ ) {
        cout << '\t' << relations[i] << endl;
    }

    cout << "Filters" << endl;
    for ( int i = 0; i < filters.size(); i++ ) {
        cout << '\t' << filters[i].rel << '\t' << filters[i].col << '\t' << (char)filters[i].op << '\t' << filters[i].constant << endl;
    }

    cout << "Joins" << endl;
    for ( int i = 0; i < joins.size(); i++ ) {
        cout << '\t' << joins[i].rel1 << '\t' << joins[i].col1 << '\t' << joins[i].rel2 << '\t' << joins[i].col2 << endl;
    }

    cout << "Projections" << endl;
    for ( int i = 0; i < selections.size(); i++ ) {
        cout << '\t' << selections[i].rel << '\t' << selections[i].col << endl;
    }
}

