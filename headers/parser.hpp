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

    // Parser class store information about a query,
    // also contain methods that are used to parse a query
    // and create all needed structs

    public:
    // Contains a vector of relations in a query
    vector<int> relations;
    // Contains all the query selections.
    vector<SelectInfo> selections;
    // Contains all the query filters.
    vector<FilterInfo> filters;
    // Contains all the query joins ( not including self joins ).
    vector<JoinInfo> joins;
    // Contains all the query self-joins.
    vector<SelfInfo> selfs;
    
    Parser(){};
    ~Parser(){};
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
