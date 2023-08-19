#ifndef EVAL_H
#define EVAL_H

#include <str.h>
#include <string_builder.h>
#include <expr.h>

#include <stdio.h>

typedef struct EvalCtx EvalCtx;

typedef struct EvalResult EvalResult;

typedef unsigned EvalStatus;
enum EvalStatus{
    EVAL_SUCCESS,
    EVAL_SYNTAX_INVALID,
    EVAL_UNEXPECTED_IDENTIFIER,
    EVAL_OUT_OF_MEMORY,
    EVAL_COULD_NOT_OPEN_FILE,
};


EvalResult eval_codegen(Str file_path);
EvalResult eval(const EvalCtx *ctx, Str source);
EvalResult eval_expr(const EvalCtx *ctx, Expr expr);
EvalResult eval_function(const EvalCtx *ctx, ExprFunction func);

void eval_result_ceanup(EvalResult *eval_result);

struct EvalCtx{
    Str main_path;
    Str main_dir;
    Str cur_dir;
    Str cur_path;

    ExprArray *assignments;
};

struct EvalResult{
    EvalStatus status;
    String *string;
};

#endif // EVAL_H
