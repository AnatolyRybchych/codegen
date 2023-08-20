#include <eval.h>
#include <expr.h>
#include <util.h>
#include <ptrarr.h>
#include <codegen_error.h>
#include <parse.h>
#include <config.h>

#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

static bool eval_expr(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, Expr expr);
static bool eval_text(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprAny text);
static bool eval_var(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprVariable var);
static bool eval_function(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);

static bool eval_for(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);
static bool eval_scope(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);
static bool eval_import(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);
static bool eval_upper(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);
static bool eval_lower(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);
static bool eval_capital(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);
static bool eval_save(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func);

static String *read_file(struct Codegen *codegen, Str path);
static bool write_file(struct Codegen *codegen, Str path, Str content);
static bool get_filesize(struct Codegen *codegen, size_t *filesize, FILE *file);

String *eval_file(struct Codegen *codegen, Str file_path){
    static String *cur_dir = NULL;
    if(!cur_dir){
        // ??? possible 2-byte allocation failure
        cur_dir = string_alloc(STR_LITERAL("."));
        assert(cur_dir != NULL && "OUT OF MEMORY");
    }

    String *file_data = read_file(codegen, file_path);
    if(file_data == NULL){
        return NULL;
    }


    Str file_str = STR(file_data->elements, file_data->elements + file_data->count);

    EvalCtx ctx = {
        .cur_dir = cur_dir,
        .assignments = NULL
    };

    String *result = eval_source(codegen, &ctx, file_str);

    free(file_data);
    free(ctx.cur_dir);

    return result;
}

String *eval_source(struct Codegen *codegen, const EvalCtx *ctx, Str source){
    ExprArray *expressions = parse_expressions(source);
    if(expressions == NULL){
        return NULL;
    }

    ExprArray *assignments = expr_array_clone(ctx->assignments);
    bool can_continue = false;

    DYN_ARRAY_FOREACH(expressions, expr){
        if(expr->type != EXPR_EMPTY && expr->type != EXPR_TEXT){
            can_continue = true;
        }

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
    can_continue = can_continue && result != NULL;

    if(expressions) free(expressions);
    if(assignments) free(assignments);

    if(can_continue){
        String *actual_result = eval_source(codegen, ctx, STR(result->elements, result->elements + result->count));
        free(result);
        return actual_result;
    }

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

static bool write_file(struct Codegen *codegen, Str path, Str content){
    char cpath[FILENAME_MAX + 1];
    if(str_len(path) >= FILENAME_MAX){
        error(codegen, STR_EMPTY, 
            "Could not read a file, path is to long (%zu)",
            (size_t)(path.end - path.beg));
        return false;
    }

    memcpy(cpath, path.beg, str_len(path));
    cpath[str_len(path)] = '\0';

    FILE *file = fopen(cpath, "w");
    if(!file){
        error(codegen, STR_EMPTY, 
            "Could not open a file '%s': %s",
            cpath, strerror(errno));
        return false;
    }

    fwrite(content.beg, 1, str_len(content), file);
    if(ferror(file)){
        error(codegen, STR_EMPTY, "Could not read a file: %s", strerror(errno));
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
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
    DYN_ARRAY_REVERSE_FOREACH(ctx->assignments, assignment){
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
    String *evaluated_args = eval_source(codegen, ctx, func.args);
    if(!evaluated_args){
        return false;
    }

    func.args = string_str(evaluated_args);

    #define _EVAL_FUNC(NAME) \
    if(str_equals(func.name, STR_LITERAL(#NAME))){\
        bool result = eval_ ## NAME(codegen, ctx, sb, func); \
        free(evaluated_args); \
        return result; \
    }

    _EVAL_FUNC(import);
    _EVAL_FUNC(scope);
    _EVAL_FUNC(for);
    _EVAL_FUNC(upper);
    _EVAL_FUNC(lower);
    _EVAL_FUNC(capital);
    _EVAL_FUNC(save);

    free(evaluated_args);

    error(codegen, func.any.bounds, "Undefined funnction '" STR_FMT "'", STR_ARG(func.name));
    return false;
}

static bool eval_for(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    (void)codegen;
    (void)ctx;

    Str iter_var = parse_name(str_ltrim(func.args));
    if(str_empty(iter_var)){
        error(codegen, func.args, "Missing c-like name for iteration variable");
        return false;
    }

    Str in_kw = parse_name(str_ltrim(STR(iter_var.end, func.args.end)));
    if(str_empty(in_kw) || !str_equals(in_kw, STR_LITERAL("in"))){
        error(codegen, func.args, "Missing 'in' keyword");
        return false;
    }

    Str it = str_ltrim(STR(in_kw.end, func.args.end));
    while (!str_empty(it)){
        Str cur;
        if(*it.beg == BLOCK_OPEN){
            cur = parse_body(it);
            if(str_empty(cur)){
                error(codegen, STR(func.any.bounds.beg, func.body.beg), "Missing %c", cpar(BLOCK_OPEN));
                return false;
            }
            it.beg = str_ltrim(STR(cur.end, it.end)).beg;
            sb_fmt(sb, "$scope{${" STR_FMT "{" STR_FMT "}}" STR_FMT "}", STR_ARG(iter_var), STR_ARG(unwrap_body(cur)), STR_ARG(func.body));
        }
        else{
            cur = parse_to_space(it);
            if(str_empty(cur)){
                return true;
            }

            it.beg = str_ltrim(STR(cur.end, it.end)).beg;
            sb_fmt(sb, "$scope{${" STR_FMT "{" STR_FMT "}}" STR_FMT "}", STR_ARG(iter_var), STR_ARG(cur), STR_ARG(func.body));
        }
    }

    return true;
}

static bool eval_scope(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    String *result = eval_source(codegen, ctx, func.body);
    if(!result){
        return false;
    }

    sb_str(sb, STR(result->elements, result->elements + result->count));
    free(result);

    return true;
}

static bool eval_import(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    Str cur_dir = string_str(ctx->cur_dir);
    Str filename = str_trim(func.args);
    String *file_path = string_alloc_fmt(STR_FMT "/" STR_FMT, STR_ARG(cur_dir), STR_ARG(filename));
    if(file_path == NULL){
        error(codegen, STR_EMPTY, "Out of memory");
        return false;
    }

    String *file_data = read_file(codegen, string_str(file_path));
    free(file_path);
    if(!file_data){
        return false;
    }

    String *result = eval_source(codegen, ctx, STR(file_data->elements, file_data->elements + file_data->count));
    free(file_data);
    
    if(!result){
        return false;
    }

    sb_str(sb, STR(result->elements, result->elements + result->count));
    free(result);

    return true;
}

static bool eval_upper(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    (void)codegen;
    (void)ctx;

    char buf[256];

    STR_EACH_CHAR(func.args, ch){
        size_t idx = (ch - func.args.beg) % sizeof(buf);
        buf[idx] = toupper(*ch);
        if(idx == 255 || ch == func.args.end - 1){
            sb_str(sb, STR(buf, buf + idx + 1));
        }
    }

    STR_EACH_CHAR(func.body, ch){
        size_t idx = (ch - func.body.beg) % sizeof(buf);
        buf[idx] = toupper(*ch);
        if(idx == 255 || ch == func.body.end - 1){
            sb_str(sb, STR(buf, buf + idx + 1));
        }
    }

    return true;
}

static bool eval_lower(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    (void)codegen;
    (void)ctx;

    char buf[256];

    STR_EACH_CHAR(func.args, ch){
        size_t idx = (ch - func.args.beg) % sizeof(buf);
        buf[idx] = tolower(*ch);
        if(idx == 255 || ch == func.args.end - 1){
            sb_str(sb, STR(buf, buf + idx + 1));
        }
    }

    STR_EACH_CHAR(func.body, ch){
        size_t idx = (ch - func.body.beg) % sizeof(buf);
        buf[idx] = tolower(*ch);
        if(idx + 1 == sizeof(buf) || ch == func.body.end - 1){
            sb_str(sb, STR(buf, buf + idx + 1));
        }
    }

    return true;
}

static bool eval_capital(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    (void)codegen;
    (void)ctx;

    if(!str_empty(func.args)){
        sb_fmt(sb, "%c" STR_FMT, toupper(*func.args.beg), STR_ARG(STR(func.args.beg + 1, func.args.end)));
    }

    if(!str_empty(func.body)){
        sb_fmt(sb, "%c" STR_FMT, toupper(*func.body.beg), STR_ARG(STR(func.body.beg + 1, func.body.end)));
    }

    return true;
}

static bool eval_save(struct Codegen *codegen, const EvalCtx *ctx, StringBuilder *sb, ExprFunction func){
    String *evaluated_body = eval_source(codegen, ctx, func.body);
    if(evaluated_body){
        Str cur_dir = string_str(ctx->cur_dir);
        Str filename = str_trim(func.args);
        String *file_path = string_alloc_fmt(STR_FMT "/" STR_FMT, STR_ARG(cur_dir), STR_ARG(filename));
        if(file_path == NULL){
            error(codegen, STR_EMPTY, "Out of memory");
            return false;
        }

        write_file(codegen, string_str(file_path), string_str(evaluated_body));
        free(file_path);
        
        sb_str(sb, string_str(evaluated_body));
        free(evaluated_body);
    }
    return evaluated_body != NULL;
}
