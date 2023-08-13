#ifndef STR_H
#define STR_H

#include <stdbool.h>

#define STR(BEG, END) (Str){.beg = BEG, .end = END}
#define STR_LITERAL(LITERAL) STR(LITERAL, LITERAL + sizeof(LITERAL) - 1)

#define STR_ARG(STR) (int)((STR).end - (STR).beg), (STR).beg
#define STR_FMT "%.*s"

typedef struct Str Str;

Str str_ltrim(Str str);
bool str_starts_with(Str str, Str substr);
Str str_str(Str str, Str substr);

struct Str{
    const char *beg;
    const char *end;
};

#endif // STR_H
