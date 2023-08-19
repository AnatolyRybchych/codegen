#include <eval.h>
#include <file.h>
#include <expr.h>
#include <util.h>
#include <ptrarr.h>

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

static EvalStatus eval_expr(const EvalCtx *ctx, StringBuilder *sb, Expr expr);
static EvalStatus eval_text(const EvalCtx *ctx, StringBuilder *sb, ExprAny text);
static EvalStatus eval_var(const EvalCtx *ctx, StringBuilder *sb, ExprVariable var);
static EvalStatus eval_function(const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);

EvalResult eval_file(Str file_path){
    FileData *file_data = filedata_from_file(file_path);
    Str file_str = STR(file_data->data, file_data->data + file_data->size);

    EvalCtx ctx = {0};
    EvalResult result = eval_source(&ctx, file_str);

    filedata_free(file_data);

    return result;
}

EvalResult eval_source(const EvalCtx *ctx, Str source){
    ExprArray *expressions = parse_expressions(source);
    if(expressions == NULL){
        return (EvalResult){
            .status = EVAL_OUT_OF_MEMORY,
            .string = NULL
        };
    }

    ExprArray *assignments = expr_array_clone(ctx->assignments);

    DYN_ARRAY_FOREACH(expressions, expr){
        if(expr->type == EXPR_ASSIGNMENT){
            assignments = expr_array_push(assignments, *expr);
        }
    }

    EvalCtx local_ctx = *ctx;
    local_ctx.assignments = assignments;

    EvalResult result = eval(&local_ctx, expressions);

    if(expressions) free(expressions);;
    if(assignments) free(assignments);;

    return result;
}

EvalResult eval(const EvalCtx *ctx, const ExprArray *expressions){
    StringBuilder sb;
    sb_init(&sb);

    DYN_ARRAY_FOREACH(expressions, expr){
        EvalStatus status = eval_expr(ctx, &sb, *expr);
        if(sb_error(&sb)){
            sb_cleanup(&sb);
            return (EvalResult){
                .status = EVAL_OUT_OF_MEMORY, 
                .string = NULL
            };
        }
        if(status != EVAL_SUCCESS){
            sb_cleanup(&sb);
            return (EvalResult){
                .status = status, 
                .string = NULL
            };
        }
    }

    String *result_str = sb_build(&sb);
    if(result_str){
        return (EvalResult){
            .status = EVAL_SUCCESS,
            .string = result_str
        };
    }
    else{
        return (EvalResult){
            .status = EVAL_OUT_OF_MEMORY,
            .string = NULL
        };
    }
}

static EvalStatus eval_expr(const EvalCtx *ctx, StringBuilder *sb, Expr expr){
    switch (expr.type){
    case EXPR_EMPTY:
    case EXPR_ASSIGNMENT:
        return EVAL_SUCCESS;
    case EXPR_FUNCTION:
        return eval_function(ctx, sb, expr.as.func);
    case EXPR_VARIABLE:
        return eval_var(ctx, sb, expr.as.var);
    case EXPR_TEXT:
        return eval_text(ctx, sb, expr.as.any);
    default:
        return assert("TODO: Not implemented" && 0), EVAL_NOT_IMPLEMENTED;
    }
}

static EvalStatus eval_text(const EvalCtx *ctx, StringBuilder *sb, ExprAny text){
    (void)ctx;
    sb_str(sb, text.bounds);
    return EVAL_SUCCESS;
}

static EvalStatus eval_var(const EvalCtx *ctx, StringBuilder *sb, ExprVariable var){
    DYN_ARRAY_FOREACH(ctx->assignments, assignment){
        ExprAssignment *asgn = &assignment->as.asgn;
        if(str_equals(asgn->name, var.name)){
            sb_str(sb, asgn->value);
            return EVAL_SUCCESS;
        }
    }
    return EVAL_UNDEFINED_VARIABLE;
}

static EvalStatus eval_function(const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    (void)ctx;

    sb_str(sb, func.any.bounds);

    return EVAL_SUCCESS;
}

void eval_result_ceanup(EvalResult *eval_result){
    if(eval_result->string){
        free(eval_result->string);
    }

    *eval_result = (EvalResult){0};
}
