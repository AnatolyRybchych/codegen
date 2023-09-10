#include <str.h>

#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <stdarg.h>

bool str_starts_with(Str str, Str substr){
    if(str_len(substr) > str_len(str)){
        return false;
    }

    for (;!str_empty(str) && !str_empty(substr); str.beg++, substr.beg++){
        if(*str.beg != *substr.beg){
            return false;
        }
    }

    return true;
}

bool str_ends_with(Str str, Str substr){
    if(str_len(substr) > str_len(str)){
        return false;
    }

    while (!str_empty(str) && !str_empty(substr)){
        str.end--, substr.end--;
        if(*str.end != *substr.end){
            return false;
        }
    }

    return true;
}

bool str_equals(Str str1, Str str2){
    size_t len = str_len(str1);
    if(len != str_len(str2)){
        return false;
    }

    return memcmp(str1.beg, str2.beg, len) == 0;
}

Str str_str(Str str, Str substr){
    while (str_len(str) >= str_len(substr)){
        if(str_starts_with(str, substr)){
            return (Str){
                .beg = str.beg,
                .end = str.beg + str_len(substr)
            };
        }
        str.beg++;
    }

    return (Str){
        .beg = str.end,
        .end = str.end
    };
}

Str str_str_r(Str str, Str substr){
    while (!str_empty(str)){
        if(str_ends_with(str, substr)){
            return (Str){
                .beg = str.end,
                .end = str.end - str_len(substr)
            };
        }
        str.end--;
    }

    return (Str){
        .beg = str.beg,
        .end = str.beg
    };
}

Str str_ltrim(Str str){
    while (!str_empty(str) && isspace(str.beg[0])){
        str.beg++;
    }
    return str;
}

Str str_rtrim(Str str){
    while (str_empty(str) && isspace(str.end[-1])){
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

String *string_alloc_uninitialized(size_t len){
    String *result = malloc(sizeof(String) + sizeof(char[len + 1]));
    if(result != NULL){
        result->elements[len] = '\0';
        result->count = len;
    }

    return result;
}

String *string_alloc_fmt(const char *fmt, ...){
    va_list args[2];
    va_start(args[0], fmt);
    va_copy(args[1], args[0]);

    int sz = vsnprintf(NULL, 0, fmt, args[0]);

    String *result = string_alloc_uninitialized(sz);
    if(result){
        vsnprintf(result->elements, result->count + 1, fmt, args[1]);
    }

    va_end(args[0]);
    va_end(args[1]);

    return result;
}

String *string_alloc(Str str){
    String *result = string_alloc_uninitialized(str_len(str));
    if(result){
        memcpy(result->elements, str.beg, sizeof(char[result->count]));
    }

    return result;
}

