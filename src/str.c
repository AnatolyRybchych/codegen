#include <str.h>

#include <ctype.h>

bool str_starts_with(Str str, Str substr){
    for (;str.beg != str.end && substr.beg != substr.end; str.beg++, substr.beg++){
        if(*str.beg != *substr.beg){
            return false;
        }
    }
    
    return true;
}

Str str_str(Str str, Str substr){
    while (str.beg != str.end){
        if(str_starts_with(str, substr)){
            return (Str){
                .beg = str.beg,
                .end = str.beg + (substr.end - substr.beg)
            };
        }
        str.beg++;
    }

    return (Str){
        .beg = str.end,
        .end = str.end
    };
}

Str str_ltrim(Str str){
    while (str.beg != str.end){
        if(isspace(*str.beg)){
            str.beg++;
        }
        else{
            return str;
        }
    }
    return str;
}
