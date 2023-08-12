#ifndef EXPR_H
#define EXPR_H

#include <str.h>
#include <stddef.h>

typedef struct Expr Expr;
typedef struct ExprArray ExprArray;

struct Expr{
    Str name;
    Str body;
};

struct ExprArray{
    size_t capacity;
    size_t count;
    Expr elemets[];
};

ExprArray *parse_expressions(Str source);

#endif // EXPR_H
