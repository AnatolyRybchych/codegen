#include <str.h>

#include <ctype.h>
#include <malloc.h>
#include <memory.h>

bool str_starts_with(Str str, Str substr){
    for (;str.beg != str.end && substr.beg != substr.end; str.beg++, substr.beg++){
        if(*str.beg != *substr.beg){
            return false;
        }
    }
    
    return true;
}

bool str_equals(Str str1, Str str2){
    long long len = str1.end - str1.beg;
    if(len != str2.end - str2.beg){
        return false;
    }

    return memcmp(str1.beg, str2.beg, len) == 0;
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
    size_t count = arr ? arr->count : 0;
    StrArr *result = realloc(arr, sizeof(StrArr) + sizeof(Str[capacity]));
    if(result != NULL){
        result->capacity = capacity;
        result->count = count;
    }

    return result;
}

StrArr *strarr_clone(StrArr *arr){
    if(arr == NULL){
        return NULL;
    }

    StrArr *result = strarr_alloc(NULL, arr->capacity);
    if(result != NULL){
        result->count = arr->count;
        memcpy(result->elements, arr->elements, sizeof(Str[arr->count]));
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

String *string_alloc(Str str){
    String *result = malloc(sizeof(String) + sizeof(char[str.end - str.beg + 1]));
    if(result == NULL){
        return NULL;
    }

    result->count = str.end - str.beg;
    memcpy(result->elements, str.beg, sizeof(char[result->count]));
    result->elements[result->count] = '\0';

    return result;
}

