#ifndef STR_H
#define STR_H

#include <stdbool.h>

#define STR(BEG, END) (Str){.beg = BEG, .end = END}
#define STR_LITERAL(LITERAL) STR(LITERAL, LITERAL + sizeof(LITERAL) - 1)

typedef struct Str Str;

Str str_ltrim(Str str);
bool str_starts_with(Str str, Str substr);
Str str_str(Str str, Str substr);

struct Str{
    const char *beg;
    const char *end;
};

#endif // STR_H
