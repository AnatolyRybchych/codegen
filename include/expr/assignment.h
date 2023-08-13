#ifndef EXPR_ASSIGNMENT_H
#define EXPR_ASSIGNMENT_H

#include <str.h>

typedef struct ExprAssignment ExprAssignment;

struct ExprAssignment{
    Str name;
    Str value;
};

/*
    "A=B" => {"A": "B"}
    "A=" => {"A": ""}
    "A" => {"A": ""}
    "A= B " => {"A": " B "}
*/
bool parse_assignment(ExprAssignment *expr, Str expr_body);

#endif // EXPR_ASSIGNMENT_H
