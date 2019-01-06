#include <cstdint>


#ifndef PARSER_INFO_H
#define PARSER_INFO_H
#include "parser_info.hpp"
#endif

#ifndef READ_H
#define READ_H
#include "read.hpp"
#endif

#ifndef RESULT_H
#define RESULT_H
#include "result.hpp"
#endif

class Filter{
    public:
    Filter(){};
    ~Filter(){};

    void filter( RelationResults & results, FilterInfo filter, FileArray & fileArray );
    void filter( MidResult & results, FilterInfo & filter, FileArray & fileArray);
};

