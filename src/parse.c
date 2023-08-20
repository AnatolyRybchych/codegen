#include <parse.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

Str parse_name(Str str){
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

Str parse_to_space(Str str){
    const char *cur = str.beg;
    while(++cur != str.end){
        if(isspace(*cur)){
            return STR(str.beg, cur);
        }
    }

    return str;
}

Str parse_body(Str str){
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

Str unwrap_body(Str str){
    if(str.beg != str.end && cpar(str.beg[0]) && cpar(str.beg[0]) == str.end[-1]){
        return STR(str.beg + 1, str.end - 1);
    }
    else{
        return STR(str.beg, str.beg);
    }
}

Expr parse_assignment(Str expr_body){
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
                .value = parse_to_space(STR(rvalue.beg + 1, rvalue.end))
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
