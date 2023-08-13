#include <eval.h>
#include <file.h>
#include <expr.h>
#include <util.h>
#include <ptrarr.h>
#include <parse.h>

#include <expr/assignment.h>
#include <expr/value.h>

#include <string.h>
#include <errno.h>
#include <assert.h>


EvalStatus eval(Str file_path, StringBuilder *output){
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
        if(str_empty(expr->name)){
            ExprValue value;
            if(parse_value(&value, unwrap_body(expr->body))){
                printf("Value: " STR_FMT "\n", STR_ARG(value.name));
            }
        }
        else if(str_equals(expr->name, STR_LITERAL("set"))){
            ExprAssignment assignment;
            if(parse_assignment(&assignment, unwrap_body(expr->body))){
                printf("Assignment: " STR_FMT " = '" STR_FMT "'\n", STR_ARG(assignment.name), STR_ARG(assignment.value));
            }
        }
    }

    filedata_free(file_data);

    return EVAL_SUCCESS;
}

EvalStatus eval_expr(const EvalCtx *ctx, const Expr *expr, StringBuilder *output){
    (void)ctx;
    (void)expr;
    (void)output;

    return EVAL_UNEXPECTED_IDENTIFIER;
}