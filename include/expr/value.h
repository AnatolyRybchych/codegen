#ifndef EXPR_VALUE_H
#define EXPR_VALUE_H

#include <str.h>

typedef struct ExprValue ExprValue;

struct ExprValue{
    Str name;
};

bool parse_value(ExprValue *expr, Str expr_body);

#endif // EXPR_VALUE_H
