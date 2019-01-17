#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <sstream>

#ifndef PARSER_INFO_H
#define PARSER_INFO_H
#include "parser_info.hpp"
#endif

using namespace std;

class Parser{
    
    public:
    vector<int> relations;
    vector<SelectInfo> selections;
    vector<FilterInfo> filters;
    vector<JoinInfo> joins;
    vector<SelfInfo> selfs;
    
    Parser(){};
    ~Parser(){};
    // parse rel col pair < x.y >
    void parseRelCol( string & rawPair );
    void parsePredicate( string & predicate );
    SelectInfo parsePair ( string & rawPair );

    void parseRelations( string & rawSelections );
    void parsePredicates( string & rawPredicates );
    void parseProjections( string & rawProjections );
    void parseQuery( string & rawQuery );

    // print method
    void printParseInfo();
};
