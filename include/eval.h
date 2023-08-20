#ifndef EVAL_H
#define EVAL_H

#include <str.h>
#include <string_builder.h>
#include <expr.h>

#include <stdio.h>

typedef struct EvalCtx EvalCtx;
typedef struct EvalResult EvalResult;
struct Codegen;

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


String *eval_file(struct Codegen *codegen, Str file_path);
String *eval_source(struct Codegen *codegen, const EvalCtx *ctx, Str source);
String *eval(struct Codegen *codegen, const EvalCtx *ctx, const ExprArray *expressions);

struct EvalCtx{
    String *cur_dir;

    ExprArray *assignments;
};

struct EvalResult{
    EvalStatus status;
    String *string;
};

#endif // EVAL_H
