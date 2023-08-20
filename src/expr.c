#include <expr.h>
#include <util.h>
#include <config.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

static Str parse_name(Str str);
static Str parse_body(Str str);
static Str unwrap_body(Str str);
static Expr parse_assignment(Str expr_body);
static Expr build_expression(Str name, Str raw, Str b1, Str b2);

static const char parenthesis[][2] = {
    {'(', ')'},
    {'{', '}'},
    {'[', ']'},
    {'<', '>'},
};

static inline char cpar(char ch){
    for(unsigned i = 0; i < ARRLEN(parenthesis); i++){
        if(parenthesis[i][0] == ch){
            return parenthesis[i][1];
        }
    }
    return 0;
}

ExprArray *parse_expressions(Str source){
    ExprArray *result = NULL;

    for (Str src = source; src.beg != src.end;){
        Str ch = str_str(src, STR_LITERAL("$"));
        Str name = parse_name(str_ltrim(STR(ch.end, src.end)));
        Str block1 = parse_body(str_ltrim(STR(name.end, src.end)));
        Str block2 = parse_body(str_ltrim(STR(block1.end, src.end)));

        Str prefix = STR(src.beg, ch.beg);
        if(!str_empty(prefix)){
            Expr text = {
                .type = EXPR_TEXT,
                .as.any.bounds = prefix
            };

            result = expr_array_push(result, text);
        }

        if(str_empty(ch)){
            break;
        }

        Expr expr = build_expression(
            name, 
            STR(ch.beg, block2.end), 
            block1,
            block2
        );

        result = expr_array_push(result, expr);
        src.beg = expr.as.any.bounds.end;
    }

    return result ? result : expr_array_alloc(NULL, 0);
}

ExprArray *expr_array_alloc(ExprArray *array, size_t capacity){
    size_t count = array ? array->count : 0;
    ExprArray *result = realloc(array, sizeof(ExprArray) + sizeof(Expr[capacity]));
    if(result == NULL){
        return result;
    }

    result->capacity = capacity;
    if(!array){
        result->count = count;
    }

    return result;
}

ExprArray *expr_array_push_any(ExprArray *array, Expr expr){
    ExprArray *result = array;

    if(array == NULL || array->count == array->capacity ){
        result = expr_array_alloc(array, array ? (array->capacity * 2 + 1) : 64);
        if(result == NULL){
            return NULL;
        }
    }

    result->elements[result->count++] = expr;
    return result;
}

ExprArray *expr_array_clone(ExprArray *arr){
    if(arr == NULL){
        return NULL;
    }

    ExprArray *result = expr_array_alloc(NULL, arr->capacity);
    if(result != NULL){
        result->count = arr->count;
        memcpy(result->elements, arr->elements, sizeof(Expr[arr->count]));
    }

    return result;
}

ExprArray *expr_array_push_function(ExprArray *array, ExprFunction function){
    return expr_array_push_any(array, (Expr){.type = EXPR_FUNCTION, .as.func = function});
}

ExprArray *expr_array_push_variable(ExprArray *array, ExprVariable variable){
    return expr_array_push_any(array, (Expr){.type = EXPR_VARIABLE, .as.var = variable});
}

ExprArray *expr_array_push_assignment(ExprArray *array, ExprAssignment assignment){
    return expr_array_push_any(array, (Expr){.type = EXPR_ASSIGNMENT, .as.asgn = assignment});
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

static Str unwrap_body(Str str){
    if(str.beg != str.end && cpar(str.beg[0]) && cpar(str.beg[0]) == str.end[-1]){
        return STR(str.beg + 1, str.end - 1);
    }
    else{
        return STR(str.beg, str.beg);
    }
}

static Expr parse_assignment(Str expr_body){
    Str name = parse_name(str_ltrim(expr_body));
    if(str_empty(name)){
        return (Expr){
            .type = EXPR_EMPTY,
            .as.any.bounds = expr_body
        };
    }

    Str rvalue = str_trim(STR(name.end, expr_body.end));
    if(str_empty(rvalue)){
        return (Expr){
            .type = EXPR_VARIABLE,
            .as.var = {
                .any.bounds = expr_body,
                .name = name
            },
        };
    }
    else if(*rvalue.beg == ':'){
        return (Expr){
            .type = EXPR_ASSIGNMENT,
            .as.asgn = {
                .any.bounds = expr_body,
                .name = name,
                .value = parse_name(STR(rvalue.beg + 1, rvalue.end))
            }
        };
    }
    else if(*rvalue.beg == '{'){
        return (Expr){
            .type = EXPR_ASSIGNMENT,
            .as.asgn = {
                .any.bounds = expr_body,
                .name = name,
                .value = unwrap_body(parse_body(rvalue))
            }
        };
    }
    else{
        return assert("TODO: Not implemented" && 0), *(Expr*)NULL;
    }
}

static Expr build_expression(Str name, Str raw, Str b1, Str b2){
    bool have_name = !str_empty(name);
    bool have_b1 = !str_empty(b1);
    bool have_b2 = !str_empty(b2);

    // $*
    if(!have_name && !have_b1){
        return (Expr){
            .type = EXPR_TEXT,
            .as.any.bounds = raw
        };
    }

    //$name *
    if(!have_b1){
        return (Expr){
            .type = EXPR_TEXT,
            .as.any.bounds = raw
        };
    }

    //$ {*}
    if(!have_name){
        Expr assignment = parse_assignment(unwrap_body(b1));
        assignment.as.any.bounds = STR(raw.beg, b1.end);
        return assignment;
    }

    // $ name (*)
    if(*b1.beg == FUNCTION_ARGS_OPEN && !have_b2){
        return (Expr){
            .type = EXPR_FUNCTION,
            .as.func = {
                .any.bounds = STR(raw.beg, b1.end),
                .name = name,
                .args = unwrap_body(b1),
                .body = STR(b1.end, b1.end)
            }
        };
    }

    // $ name (*) {*}
    if(*b1.beg == FUNCTION_ARGS_OPEN && have_b2 && *b2.beg == FUNCTION_BLOCK_OPEN){
        return (Expr){
            .type = EXPR_FUNCTION,
            .as.func = {
                .any.bounds = raw,
                .name = name,
                .args = unwrap_body(b1),
                .body = unwrap_body(b2)
            }
        };
    }

    // $ name {*}
    if(*b1.beg == FUNCTION_BLOCK_OPEN){
        return (Expr){
            .type = EXPR_FUNCTION,
            .as.func = {
                .any.bounds = STR(raw.beg, b1.end),
                .name = name,
                .args = STR(b1.beg, b1.beg),
                .body = unwrap_body(b1)
            }
        };
    }

    return (Expr){
        .type = EXPR_TEXT,
        .as.any.bounds = raw
    };
}
