#include <codegen_error.h>
#include <codegen.h>
#include <ptrarr.h>
#include <util.h>
#include <parse.h>

#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

void error(struct Codegen *codegen, Str chunk, const char *fmt, ...){
    va_list args[2];
    va_start(args[0], fmt);
    va_copy(args[1], args[0]);

    int msg_len = vsnprintf(NULL, 0, fmt, args[0]);

    CodegenError *err = malloc(sizeof(CodegenError) + sizeof(char[msg_len + 1]));
    if(err){
        err->chunk = str_empty(chunk) ? NULL : string_alloc(chunk);
        err->count = msg_len;
        
        vsnprintf(err->elements, msg_len + 1, fmt, args[1]);

        PtrArr *errors = ptrarr_push(codegen->errors, err);
        assert(errors != NULL && "Could not handle error beacuse of out of memory");
        codegen->errors = errors;
    }

    va_end(args[0]);
    va_end(args[1]);
}

void error_free(CodegenError *error){
    if(error->chunk){
        free(error->chunk);
    }
    free(error);
}

void print_errors(struct Codegen *codegen){
    DYN_ARRAY_FOREACH(codegen->errors, _err){
        CodegenError *err = *(CodegenError **)_err;
        fprintf(stderr, "\033[0;31m%s\033[0m\n", err->elements);
        if(err->chunk){
            Str chunk_to_display = parse_to_space(string_str(err->chunk));
            fprintf(stderr, "    " STR_FMT "\n", STR_ARG(chunk_to_display));
            fprintf(stderr, "    ");

            STR_EACH_CHAR(chunk_to_display, ch){
                fprintf(stderr, "^");
            }
            fprintf(stderr, "\n");
        }
    }
}

bool is_error(struct Codegen *codegen){
    return codegen->errors != NULL && codegen->errors->count != 0;
}
