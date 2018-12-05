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

// void Parser::parseRelations( string & rawSelections ) {
//     // Parse selections
//     char * token;
//     vector<string> words;
//     char * str = strdup(rawSelections.c_str());
//     // Split string into words
//     token = strtok(str," ");
//     cout << token << endl;
//     selections.push_back(atoi(token));
//     while ( (token = strtok(NULL," ")) != NULL ) {
//         cout << token << endl;
//         selections.push_back(atoi(token));
//     }
//     free(str);
// }

void Parser::parseRelations( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,' ');
    for ( int i = 0; i < words.size(); i++ ) {
        cout << words[i] << endl;
    }
}

void Parser::parsePredicates( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,'&');
    for ( int i = 0; i < words.size(); i++ ) {
        cout << words[i] << endl;
    }
}

void Parser::parseProjections( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,' ');
    for ( int i = 0; i < words.size(); i++ ) {
        cout << words[i] << endl;
    }
}

void Parser::parseQuery( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,'|');
    for ( int i = 0; i < words.size(); i++ ) {
        cout << words[i] << endl;
    }
    cout << "now parsing relations" << endl;
    parseRelations(words[0]);
    cout << "now parsing predicates" << endl;
    parsePredicates(words[1]);
    cout << "now parsing projections" << endl;
    parseProjections(words[2]);
}

int main ( void ) {

    string str = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1";
    
    Parser parser;
    parser.parseQuery(str);
}