#ifndef PARSE_H
#define PARSE_H

#include <str.h>
#include <util.h>

Str parse_name(Str str);
Str parse_body(Str str);
Str unwrap_body(Str str);

static const char parenthesis[][2] = {
    {'(', ')'},
    {'{', '}'},
    {'[', ']'},
    {'<', '>'},
};

static inline char cpar(char ch){
    for(unsigned i = 0; i < ARRLEN(parenthesis); i++){
        if(parenthesis[i][0] == ch){
            return parenthesis[i][1];
        }
    }
    return 0;
}

#endif // PARSE_H
