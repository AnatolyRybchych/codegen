#ifndef EVAL_H
#define EVAL_H

#include <str.h>
#include <string_builder.h>
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


EvalStatus eval(Str file_path, StringBuilder *output);
EvalStatus eval_expr(const EvalCtx *ctx, const Expr *expr, StringBuilder *output);

struct EvalCtx{
    Str main_path;
    Str main_dir;
    Str cur_dir;
    Str cur_path;

    StrArr *scope;
    StrArr *variables;
};

#endif // EVAL_H
