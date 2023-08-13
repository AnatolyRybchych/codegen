#include <parse.h>

#include <ctype.h>
#include <memory.h>
#include <assert.h>

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
