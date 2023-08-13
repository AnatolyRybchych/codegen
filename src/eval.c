#include <eval.h>
#include <file.h>
#include <expr.h>
#include <util.h>
#include <ptrarr.h>
#include <parse.h>

#include <string.h>
#include <errno.h>
#include <assert.h>



EvalStatus eval(Str file_path, FILE *output){
    (void)output;

    FileData *file_data = filedata_from_file(file_path);
    Str file_str = STR(file_data->data, file_data->data + file_data->size);

    ExprArray *expressions = parse_expressions(file_str);
    if(expressions == NULL || expressions->count == 0){
        return EVAL_SUCCESS;
    }

    PtrArr *child_expressions = ptrarr_alloc(NULL, expressions->count);
    if(child_expressions == NULL){
        return EVAL_OUT_OF_MEMORY;
    }

    DYN_ARRAY_FOREACH(expressions, expr){
        if(str_equals(expr->name, STR_LITERAL("set"))){
            Expr assignment;
            if(eval_assignment(expr->body, &assignment) == EVAL_SUCCESS){
                printf(STR_FMT " = '" STR_FMT "'\n", STR_ARG(assignment.name), STR_ARG(assignment.body));
            }
        }
    }

    filedata_free(file_data);

    return EVAL_SUCCESS;
}

EvalStatus eval_assignment(Str assignment, Expr *assignment_expr){
    if(assignment.beg == assignment.end){
        return EVAL_SYNTAX_INVALID;
    }

    if(cpar(*assignment.beg)){
        assignment = parse_body(assignment);
        if(assignment.end == assignment.beg){
            return EVAL_SYNTAX_INVALID;
        }

        assignment = STR(assignment.beg + 1, assignment.end - 1);
    }

    Str name = parse_name(str_ltrim(assignment));
    if(name.beg == name.end){
        return EVAL_SYNTAX_INVALID;
    }

    Str rvalue = str_ltrim(STR(name.end, assignment.end));
    if(rvalue.beg == rvalue.end){
        *assignment_expr = (Expr){
            .name = name,
            .body = rvalue,
        };
        return EVAL_SUCCESS;
    }
    else if(*rvalue.beg == '='){
        rvalue = STR(rvalue.beg + 1, rvalue.end);
        *assignment_expr = (Expr){
            .name = name,
            .body = rvalue,
        };
        return EVAL_SUCCESS;
    }
    else{
        return EVAL_SYNTAX_INVALID;
    }
}

EvalStatus eval_expr(const EvalCtx *ctx, const Expr *expr, FILE *output){
    (void)ctx;
    (void)expr;
    (void)output;

    return EVAL_UNEXPECTED_IDENTIFIER;
}