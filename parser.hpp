#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
 
class Parser {

    public:
    // Vector of selections
    std::vector<uint64_t> selections;

    
    public:
    Parser(){};
    ~Parser(){};

    void parseRelations( std::string & rawSelections );
    void parsePredicates( std::string & rawPredicates );
    void parseProjections( std::string & rawProjections );
    void parseQuery( std::string & rawQuery );
};