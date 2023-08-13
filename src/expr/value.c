#include <expr/value.h>
#include <parse.h>

bool parse_value(ExprValue *expr, Str expr_body){
    if(expr_body.beg == expr_body.end){
        return false;
    }

    Str name = parse_name(str_trim(expr_body));
    if(name.beg == name.end){
        return false;
    }

    *expr = (ExprValue){
        .name = name
    };

    return true;
}
