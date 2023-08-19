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
    EVAL_UNDEFINED_VARIABLE,
    EVAL_NOT_IMPLEMENTED,
    EVAL_SYNTAX_INVALID,
    EVAL_UNEXPECTED_IDENTIFIER,
    EVAL_OUT_OF_MEMORY,
    EVAL_COULD_NOT_OPEN_FILE,
};


EvalResult eval_file(Str file_path);
EvalResult eval_source(const EvalCtx *ctx, Str source);
EvalResult eval(const EvalCtx *ctx, const ExprArray *expressions);

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
