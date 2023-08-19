#ifndef EXPR_H
#define EXPR_H

#include <str.h>
#include <stddef.h>

#define expr_array_push(array, expr) _Generic((expr), \
    Expr: expr_array_push_any,\
    ExprFunction: expr_array_push_function, \
    ExprVariable: expr_array_push_variable, \
    ExprAssignment: expr_array_push_assignment)(array, expr)

typedef unsigned ExprType;
enum ExprType{
    EXPR_EMPTY,
    EXPR_TEXT,
    EXPR_FUNCTION,
    EXPR_VARIABLE,
    EXPR_ASSIGNMENT,
};

typedef struct Expr Expr;
typedef struct ExprAny ExprAny;
typedef struct ExprFunction ExprFunction;
typedef struct ExprVariable ExprVariable;
typedef struct ExprAssignment ExprAssignment;

typedef struct ExprArray ExprArray;

struct ExprAny{
    Str bounds;
};

struct ExprFunction{
    ExprAny any;
    Str name;
    Str body;
};

struct ExprVariable{
    ExprAny any;
    Str name;
};

struct ExprAssignment{
    ExprAny any;
    Str name;
    Str value;
};


struct Expr{
    ExprType type;
    union ExprUnion{
        ExprAny any;
        ExprFunction func;
        ExprVariable var;
        ExprAssignment asgn;
    } as;
};

struct ExprArray{
    size_t capacity;
    size_t count;
    Expr elements[];
};

ExprArray *parse_expressions(Str source);
ExprArray *expr_array_alloc(ExprArray *array, size_t capacity);

ExprArray *expr_array_clone(ExprArray *arr);

ExprArray *expr_array_push_any(ExprArray *array, Expr expr);
ExprArray *expr_array_push_function(ExprArray *array, ExprFunction function);
ExprArray *expr_array_push_variable(ExprArray *array, ExprVariable variable);
ExprArray *expr_array_push_assignment(ExprArray *array, ExprAssignment assignment);

#endif // EXPR_H
