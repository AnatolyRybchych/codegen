#include <eval.h>
#include <expr.h>
#include <util.h>
#include <ptrarr.h>
#include <codegen_error.h>

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static bool eval_expr(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, Expr expr);
static bool eval_text(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprAny text);
static bool eval_var(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprVariable var);
static bool eval_function(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);

static String *read_file(struct Codegen *codegen, Str path);
static bool get_filesize(struct Codegen *codegen, size_t *filesize, FILE *file);

String *eval_file(struct Codegen *codegen, Str file_path){
    String *file_data = read_file(codegen, file_path);
    if(file_data == NULL){
        return NULL;
    }

    Str file_str = STR(file_data->elements, file_data->elements + file_data->count);

    EvalCtx ctx = {0};
    String *result = eval_source(codegen, &ctx, file_str);
    free(file_data);

    return result;
}

String *eval_source(struct Codegen *codegen, const EvalCtx *ctx, Str source){
    ExprArray *expressions = parse_expressions(source);
    if(expressions == NULL){
        return NULL;
    }

    ExprArray *assignments = expr_array_clone(ctx->assignments);

    DYN_ARRAY_FOREACH(expressions, expr){
        if(expr->type == EXPR_ASSIGNMENT){
            assignments = expr_array_push(assignments, *expr);
            if(assignments == NULL){
                free(expressions);
                error(codegen, STR_EMPTY, "Out of memory");
                return NULL;
            }
        }
    }

    EvalCtx local_ctx = *ctx;
    local_ctx.assignments = assignments;

    String *result = eval(codegen, &local_ctx, expressions);

    if(expressions) free(expressions);;
    if(assignments) free(assignments);;

    return result;
}

String *eval(struct Codegen *codegen, const EvalCtx *ctx, const ExprArray *expressions){
    StringBuilder sb;
    sb_init(&sb);

    DYN_ARRAY_FOREACH(expressions, expr){
        if(!eval_expr(codegen, ctx, &sb, *expr)){
            return NULL;
        }
        if(sb_out_of_memory(&sb)){
            error(codegen, STR_EMPTY, "Out of memory");
            return NULL;
        }
    }

    String *result_str = sb_build(&sb);
    sb_cleanup(&sb);

    if(result_str == NULL){
        error(codegen, STR_EMPTY, "Out of memory");
    }
    return result_str;
}

static String *read_file(struct Codegen *codegen, Str path){
    char cpath[FILENAME_MAX + 1];
    if(str_len(path) >= FILENAME_MAX){
        error(codegen, STR_EMPTY, 
            "Could not read a file, path is to long (%zu)",
            (size_t)(path.end - path.beg));
        return NULL;
    }

    memcpy(cpath, path.beg, str_len(path));
    cpath[str_len(path)] = '\0';

    FILE *file = fopen(cpath, "r");
    if(!file){
        error(codegen, STR_EMPTY, 
            "Could not open a file '%s': %s",
            cpath, strerror(errno));
        return NULL;
    }

    size_t filesize;
    if(get_filesize(codegen, &filesize, file)){
        String *data = string_alloc_uninitialized(filesize);
        fread(data->elements, 1, filesize, file);
        if(ferror(file)){
            error(codegen, STR_EMPTY, "Could not read a file: %s", strerror(errno));

            free(data);
            data = NULL;
        }

        fclose(file);
        return data;
    }

    fclose(file);
    return NULL;
}

static bool get_filesize(struct Codegen *codegen, size_t *filesize, FILE *file){
    int cursor_prev = ftell(file);
    if(cursor_prev < 0){
        error(codegen, STR_EMPTY, "Could not get cursor position of a file");
    }

    if(fseek(file, 0, SEEK_END) < 0){
        error(codegen, STR_EMPTY, "Could not move cursor to the end of the file");
        return false;
    }

    int pos = ftell(file);
    if(pos < 0){
        error(codegen, STR_EMPTY, "Could not get cursor position of the end of a file");
        return false;
    }
    *filesize = pos;

    if(cursor_prev >= 0 && fseek(file, cursor_prev, SEEK_SET) < 0){
        error(codegen, STR_EMPTY, "Could not restore cursor position after measuring a file");
    }

    return true;
}

static bool eval_expr(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, Expr expr){
    switch (expr.type){
    case EXPR_EMPTY:
    case EXPR_ASSIGNMENT:
        return true;
    case EXPR_FUNCTION:
        return eval_function(codegen, ctx, sb, expr.as.func);
    case EXPR_VARIABLE:
        return eval_var(codegen, ctx, sb, expr.as.var);
    case EXPR_TEXT:
        return eval_text(codegen, ctx, sb, expr.as.any);
    default:
        return assert("TODO: Not implemented" && 0), false;
    }
}

static bool eval_text(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprAny text){
    (void)codegen;
    (void)ctx;
    sb_str(sb, text.bounds);
    return true;
}

static bool eval_var(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprVariable var){
    DYN_ARRAY_FOREACH(ctx->assignments, assignment){
        ExprAssignment *asgn = &assignment->as.asgn;
        if(str_equals(asgn->name, var.name)){
            sb_str(sb, asgn->value);
            return true;
        }
    }

    error(codegen, var.name, "Using of undefined variable");
    return false;
}

static bool eval_function(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    (void)ctx;
    (void)sb;

    error(codegen, func.any.bounds, "Undefined funnction '" STR_FMT "'", STR_ARG(func.name));
    return false;
}

