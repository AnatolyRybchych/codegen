#include <str.h>

#include <ctype.h>
#include <malloc.h>

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
    while (str.beg != str.end && isspace(str.beg[0])){
        str.beg++;
    }
    return str;
}

Str str_rtrim(Str str){
    while (str.end != str.beg && isspace(str.end[-1])){
        str.end--;
    }
    return str;
}

Str str_trim(Str str){
    return str_ltrim(str_rtrim(str));
}

StrArr *strarr_alloc(StrArr *arr, size_t capacity){
    StrArr *result = realloc(arr, sizeof(StrArr) + sizeof(Str[capacity]));
    if(result != NULL){
        result->capacity = capacity;
        result->count = 0;
    }

    return result;
}

StrArr *strarr_push(StrArr *arr, Str element){
    StrArr *result = arr;
    if(arr == NULL || arr->count == arr->capacity){
        result = strarr_alloc(arr, arr? arr->capacity * 2 + 1 : 4);
        if(result == NULL){
            return NULL;
        }
    }

    result->elements[result->count++] = element;
    return result;
}

