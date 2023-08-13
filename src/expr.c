#include <expr.h>
#include <util.h>
#include <parse.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

static ExprArray *expr_array_alloc(ExprArray *array, size_t capacity);
static ExprArray *expr_array_push(ExprArray *array, Str name, Str body);

ExprArray *parse_expressions(Str source){
    ExprArray *result = NULL;

    const char *cur = source.beg;
    while (cur != source.end){
        Str expr_ch = str_str(STR(cur, source.end), STR_LITERAL("$"));
        if(expr_ch.beg == expr_ch.end){
            return result;
        }

        Str expr_name = parse_name(str_ltrim(STR(expr_ch.end, source.end)));
        Str expr_body = parse_body(str_ltrim(STR(expr_name.end, source.end)));

        if(expr_body.beg != expr_body.end){
            result = expr_array_push(result, expr_name, expr_body);
        }
        cur = expr_body.end;
    }

    return result;
}

static ExprArray *expr_array_alloc(ExprArray *array, size_t capacity){
    ExprArray *result = realloc(array, sizeof(ExprArray) + sizeof(Expr[capacity]));
    if(result == NULL){
        return result;
    }

    result->capacity = capacity;
    if(!array){
        result->count = 0;
    }

    return result;
}

static ExprArray *expr_array_push(ExprArray *array, Str name, Str body){
    ExprArray *result = array;

    if(array == NULL || array->count == array->capacity ){
        result = expr_array_alloc(array, array ? (array->capacity * 2 + 1) : 64);
        if(result == NULL){
            return NULL;
        }
    }

    result->elements[result->count++] = (Expr){
        .name = name,
        .body = body
    };

    return result;
}
