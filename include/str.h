#ifndef STR_H
#define STR_H

#include <stdbool.h>
#include <stddef.h>

#define STR(BEG, END) (Str){.beg = BEG, .end = END}
#define STR_LITERAL(LITERAL) STR(LITERAL, LITERAL + sizeof(LITERAL) - 1)

#define STR_ARG(STR) (int)((STR).end - (STR).beg), (STR).beg
#define STR_FMT "%.*s"

typedef struct Str Str;
typedef struct String String;
typedef struct StrArr StrArr;

Str str_ltrim(Str str);
Str str_rtrim(Str str);
Str str_trim(Str str);
bool str_starts_with(Str str, Str substr);
bool str_equals(Str str1, Str str2);
Str str_str(Str str, Str substr);

StrArr *strarr_alloc(StrArr *arr, size_t capacity);
StrArr *strarr_push(StrArr *arr, Str element);

struct Str{
    const char *beg;
    const char *end;
};

struct StrArr{
    size_t capacity;
    size_t count;
    Str elements[];
};

struct String{
    size_t count;
    char elements[];
};

static inline bool str_empty(Str str){
    return str.beg == str.end;
}

#endif // STR_H
