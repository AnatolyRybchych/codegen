#include <string_builder.h>
#include <util.h>

#include <malloc.h>
#include <stdarg.h>

void sb_init(StringBuilder *sb){
    *sb = (StringBuilder){0};
}

bool sb_out_of_memory(StringBuilder *sb){
    return sb->out_of_memory;
}

String *sb_build(StringBuilder *sb){
    size_t size = 0;
    DYN_ARRAY_FOREACH(sb->strings, _string){
        String *string = *(String**)_string; 
        size += string->count;
    }

    String *result = malloc(sizeof(String) + sizeof(char[size + 1]));
    if(result != NULL){
        char *dst_cur = result->elements;
        DYN_ARRAY_FOREACH(sb->strings, _string){
            String *string = *_string;
            memcpy(dst_cur, string->elements, sizeof(char[string->count]));
            dst_cur += string->count;
        }
        *dst_cur = '\0';
        result->count = size;
    }

    return result;
}

void sb_write(StringBuilder *sb, FILE *stream){
    DYN_ARRAY_FOREACH(sb->strings, _string){
        String *string = *_string;
        fwrite(string->elements, 1, string->count, stream);
    }
}

void sb_cleanup(StringBuilder *sb){
    DYN_ARRAY_FOREACH(sb->strings, string){
        free(*string);
    }

    if(sb->strings){
        free(sb->strings);
    }
    sb->out_of_memory = false;
}


void sb_fmt(StringBuilder *sb, const char *fmt, ...){
    va_list args[2];
    va_start(args[0], fmt);
    va_copy(args[1], args[0]);

    int sz = vsnprintf(NULL, 0, fmt, args[0]);

    String *string = malloc(sizeof(String) + sizeof(char[sz + 1]));
    if(string == NULL){
        sb->out_of_memory = true;
        return;
    }
    
    string->count = sz;
    vsnprintf(string->elements, string->count, fmt, args[1]);
    string->elements[string->count] = '\0';

    va_end(args[0]);
    va_end(args[1]);

    PtrArr *strings = ptrarr_push(sb->strings, string);
    if(strings == NULL){
        free(string);
        sb->out_of_memory = true;
        return;
    }

    sb->strings = strings;
}

void sb_str(StringBuilder *sb, Str str){
    size_t len = str_len(str);
    String *string = malloc(sizeof(String) + sizeof(char[len + 1]));
    if(string == NULL){
        sb->out_of_memory = true;
        return;
    }

    string->count = len;
    memcpy(string->elements, str.beg, string->count);
    string->elements[string->count] = '\0';

    PtrArr *strings = ptrarr_push(sb->strings, string);
    if(strings == NULL){
        free(string);
        sb->out_of_memory = true;
        return;
    }

    sb->strings = strings;
}

void sb_cstr(StringBuilder *sb, char *cstr){
    sb_str(sb, STR(cstr, cstr + strlen(cstr)));
}
