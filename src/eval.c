#include <eval.h>
#include <file.h>
#include <expr.h>
#include <util.h>
#include <ptrarr.h>

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#define ERROR(ERROR) (EvalResult){.status = EVAL_ ## ERROR, .string = NULL}
#define SUCCESS(STRING) (EvalResult){.status = EVAL_SUCCESS, .string = STRING}

EvalResult eval_codegen(Str file_path){
    FileData *file_data = filedata_from_file(file_path);
    Str file_str = STR(file_data->data, file_data->data + file_data->size);

    EvalCtx ctx = {0};
    EvalResult result = eval(&ctx, file_str);

    filedata_free(file_data);

    return result;
}

EvalResult eval(const EvalCtx *ctx, Str source){
    ExprArray *expressions = parse_expressions(source);
    if(expressions == NULL){
        return SUCCESS(NULL);
    }

    ExprArray *assignments = expr_array_clone(ctx->assignments);

    DYN_ARRAY_FOREACH(expressions, expr){
        if(expr->type == EXPR_ASSIGNMENT){
            assignments = expr_array_push(assignments, *expr);
        }
    }

    EvalCtx local_ctx = *ctx;
    local_ctx.assignments = assignments;

    StringBuilder sb;
    sb_init(&sb);

    DYN_ARRAY_FOREACH(expressions, expr){
        if(expr->type == EXPR_ASSIGNMENT){
            continue;
        }
        EvalResult result = eval_expr(&local_ctx, *expr);
        if(result.status == EVAL_SUCCESS){
            if(result.string){
                sb_str(&sb, STR(result.string->elements, result.string->elements + result.string->count));
            }
        }
        else{
            sb_cleanup(&sb);
            if(expressions) free(expressions);
            if(assignments) free(assignments);
            return result;
        }
    }

    String *result = sb_build(&sb);

    sb_cleanup(&sb);
    if(expressions) free(expressions);
    if(assignments) free(assignments);

    return SUCCESS(result);
}

EvalResult eval_expr(const EvalCtx *ctx, Expr expr){
    String *s;
    switch (expr.type){
    case EXPR_EMPTY:
    case EXPR_ASSIGNMENT:
        return SUCCESS(NULL);
    case EXPR_FUNCTION:
        return eval_function(ctx, expr.as.func);
    case EXPR_VARIABLE:
        DYN_ARRAY_FOREACH(ctx->assignments, assignment){
            ExprAssignment *asgn = &assignment->as.asgn;
            if(str_equals(asgn->name, expr.as.var.name)){
                s = string_alloc(asgn->value);
                return s ? SUCCESS(s) : ERROR(OUT_OF_MEMORY);
            }
        }

        s = string_alloc(expr.as.any.bounds);
        return s ? SUCCESS(s) : ERROR(OUT_OF_MEMORY);
    case EXPR_TEXT:
        s = string_alloc(expr.as.any.bounds);
        return s ? SUCCESS(s) : ERROR(OUT_OF_MEMORY);
    default:
        return assert("TODO: Not implemented" && 0), *(EvalResult*)NULL;
    }
}

EvalResult eval_function(const EvalCtx *ctx, ExprFunction func){
    (void)ctx;

    String *s = string_alloc(func.any.bounds);
    return s ? SUCCESS(s) : ERROR(OUT_OF_MEMORY);
}

void eval_result_ceanup(EvalResult *eval_result){
    if(eval_result->string){
        free(eval_result->string);
    }

    *eval_result = (EvalResult){0};
}
