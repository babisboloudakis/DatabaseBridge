#ifndef READ_H
#define READ_H
#include "read.hpp"
#endif

#include "parser.hpp"

class Optimize{
    public:

    Optimize(){};
    ~Optimize(){};

    void optimizeQuery(FileArray & fileArray, Parser & parser);
};