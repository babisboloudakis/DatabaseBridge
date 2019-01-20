#include "headers/parser.hpp"

using namespace std;

static void splitString(string& line, vector<string>& result, const char delim) {
    // Generic split string function used to break down query sections
    // using given delimeters.
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
    selections.emplace_back( SelectInfo( relations[stoul(words[0])] , stoul(words[1]), stoul(words[0]) ) );
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
    return SelectInfo( relations[stoul(words[0])] , stoul(words[1]), stoul(words[0]) );
}

// Determines if a given string is constant or not.
inline static bool isConstant(string& raw) { return raw.find('.')==string::npos; }

void Parser::parsePredicate(string& rawPredicate) {
    vector<string> preds;
    splitPredicates(rawPredicate, preds);
    // Read first pair.
    SelectInfo left = parsePair(preds[0]);
    if ( isConstant(preds[1]) ) { 
        // filter operation
        char compType=rawPredicate[preds[0].size()];
        filters.emplace_back( FilterInfo( left.rel, left.col, stoul(preds[1]), FilterInfo::FilterOperation(compType), left.index ) );
    } else { 
        // Join operation
        SelectInfo right = parsePair(preds[1]);
        // Join can either be normal join or self join. We store them seperately.
        if ( left.index == right.index ) {
            // Self Join.
            selfs.emplace_back( SelfInfo( left.rel, left.col, right.col, left.index ) );
        } else {
            // Regular Join.
            joins.emplace_back( JoinInfo( left.rel, left.col, right.rel, right.col, left.index, right.index ) );
        }
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
    // Parse each query section.
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
        cout << '\t' << filters[i].rel << '\t' << filters[i].col << '\t' << (char)filters[i].op << '\t' << filters[i].constant << '\t' <<  filters[i].index <<  endl;
    }
    cout << "Joins" << endl;
    for ( int i = 0; i < joins.size(); i++ ) {
        cout << '\t' << joins[i].rel1 << '\t' << joins[i].col1 << '\t' << joins[i].rel2 << '\t' << joins[i].col2 << joins[i].index1 << '\t' << joins[i].index2 << endl;
    }
    cout << "Selfs" << endl;
    for (int i = 0; i < selfs.size(); i++) {
        cout << '\t' << selfs[i].rel << '\t' << selfs[i].col1 << '\t' << selfs[i].col2 << '\t' <<  selfs[i].index <<  endl;
    }
    cout << "Projections" << endl;
    for ( int i = 0; i < selections.size(); i++ ) {
        cout << '\t' << selections[i].rel << '\t' << selections[i].col << endl;
    }
}