#include <expr/assignment.h>
#include <parse.h>

#include <stdio.h>

bool parse_assignment(ExprAssignment *expr, Str expr_body){
    if(expr_body.beg == expr_body.end){
        return false;
    }

    Str name = parse_name(str_ltrim(expr_body));
    if(name.beg == name.end){
        return false;
    }

    Str rvalue = str_ltrim(STR(name.end, expr_body.end));
    if(rvalue.beg == rvalue.end){
        *expr = (ExprAssignment){
            .name = name,
            .value = rvalue,
        };
        return true;
    }
    else if(*rvalue.beg == '='){
        rvalue = STR(rvalue.beg + 1, rvalue.end);
        *expr = (ExprAssignment){
            .name = name,
            .value = rvalue,
        };
        return true;
    }
    else{
        return false;
    }
}
