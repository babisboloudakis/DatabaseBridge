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

void Parser::parseRelCol ( string & rawPair ) { 
    // Parse rel col pair
    vector<string> words;
    // Split string into words
    splitString(rawPair,words,'.');
    cout << words[0] << "rel " << words[1] << "col" << endl;
    selections.emplace_back( SelectInfo( atoi(words[0].c_str()) , atoi(words[1].c_str()) ) );
}

void Parser::parseRelations( string & rawSelections ) {
    // Parse selections
    vector<string> words;
    // Split string into words
    splitString(rawSelections,words,' ');
    for ( int i = 0; i < words.size(); i++ ) {
        relations.push_back( atoi( words[i].c_str() ) );
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
        parseRelCol(words[i]);
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
    cout << relations.size() << endl;
    cout << "now parsing predicates" << endl;
    parsePredicates(words[1]);
    cout << "now parsing projections" << endl;
    parseProjections(words[2]);
    cout << selections.size() << endl;
    cout << selections[1].col << endl;
}

int main ( void ) {

    string str = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1";
    
    Parser parser;
    parser.parseQuery(str);
}