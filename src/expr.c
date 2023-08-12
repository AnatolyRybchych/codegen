#include <expr.h>
#include <util.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

static ExprArray *expr_array_alloc(ExprArray *array, size_t capacity);
static ExprArray *expr_array_push(ExprArray *array, Str name, Str body);
static Str parse_name(Str str);
static char cpar(char ch);
static Str parse_body(Str str);

static const char parenthesis[][2] = {
    {'(', ')'},
    {'{', '}'},
    {'[', ']'},
    {'<', '>'},
};

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
        printf("$%.*s%.*s\n", (int)(expr_name.end - expr_name.beg), expr_name.beg,
            (int)(expr_body.end - expr_body.beg), expr_body.beg);
        
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

    result->elemets[result->count++] = (Expr){
        .name = name,
        .body = body
    };

    return result;
}


static Str parse_name(Str str){
    if(!isalpha(*str.beg) && *str.beg != '_'){
        return STR(str.beg, str.beg);
    }

    const char *cur = str.beg;
    while(++cur != str.end){
        if(!isalnum(*cur) && *cur != '_'){
            return STR(str.beg, cur);
        }
    }

    return str;
}

static char cpar(char ch){
    for(unsigned i = 0; i < ARRLEN(parenthesis); i++){
        if(parenthesis[i][0] == ch){
            return parenthesis[i][1];
        }
    }
    return 0;
}

static Str parse_body(Str str){
    if(!cpar(*str.beg)){
        return STR(str.beg, str.beg);
    }

    int stack_cur = -1;
    char stack[1024];
    memset(stack, 0, sizeof(stack));

    const char *cur = str.beg;
    for (;cur != str.end; cur++){
        char paren = cpar(*cur); 
        if(paren != 0){
            assert(stack_cur + 1 != ARRLEN(stack));
            stack[++stack_cur] = paren;
        }
        else if(stack[stack_cur] == *cur){
            if(stack_cur-- == 0){
                return STR(str.beg, cur + 1);
            }
        }
    }

    return str;
}