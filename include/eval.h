#ifndef EVAL_H
#define EVAL_H

#include <str.h>
#include <expr.h>

#include <stdio.h>

typedef struct EvalCtx EvalCtx;

typedef unsigned EvalStatus;
enum EvalStatus{
    EVAL_SUCCESS,
    EVAL_SYNTAX_INVALID,
    EVAL_UNEXPECTED_IDENTIFIER,
    EVAL_OUT_OF_MEMORY,
    EVAL_COULD_NOT_OPEN_FILE,
};


EvalStatus eval(Str file_path, FILE *output);

/*
    "A=B" => {"A": "B"}
    "A=" => {"A": ""}
    "A" => {"A": ""}
    "A= B " => {"A": " B "}
*/

EvalStatus eval_assignment(Str assignment, Expr *assignment_expr);
EvalStatus eval_expr(const EvalCtx *ctx, const Expr *expr, FILE *output);

struct EvalCtx{
    Str main_path;
    Str main_dir;
    Str cur_dir;
    Str cur_path;

    StrArr *scope;
    StrArr *variables;
};

#endif // EVAL_H
