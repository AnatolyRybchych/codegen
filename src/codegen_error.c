#include <codegen_error.h>
#include <codegen.h>
#include <ptrarr.h>
#include <util.h>

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
        err->chunk = chunk;
        err->count = msg_len;
        
        vsnprintf(err->elements, msg_len + 1, fmt, args[1]);

        PtrArr *errors = ptrarr_push(codegen->errors, err);
        assert(errors != NULL && "Could not handle error beacuse of out of memory");
        codegen->errors = errors;
    }

    va_end(args[0]);
    va_end(args[1]);
}

void print_errors(struct Codegen *codegen){
    DYN_ARRAY_FOREACH(codegen->errors, _err){
        CodegenError *err = *(CodegenError **)_err;
        fprintf(stderr, "ERROR: %s\n", err->elements);
        if(!str_empty(err->chunk)){
            fprintf(stderr, "    " STR_FMT "\n", STR_ARG(err->chunk));
            fprintf(stderr, "    ");
            for(const char *c = err->chunk.beg; c != err->chunk.end; c++){
                fprintf(stderr, "^");
            }
            fprintf(stderr, "\n\n");
        }
    }
}

bool is_error(struct Codegen *codegen){
    return codegen->errors != NULL && codegen->errors->count != 0;
}
