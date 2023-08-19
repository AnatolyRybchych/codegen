#ifndef CODEGEN_ERROR_H
#define CODEGEN_ERROR_H

#include <str.h>

typedef struct CodegenError CodegenError;
struct Codegen;

void error(struct Codegen *codegen, Str chunk, const char *fmt, ...);
void print_errors(struct Codegen *codegen);
bool is_error(struct Codegen *codegen);

struct CodegenError{
    Str chunk;

    size_t count;
    char elements[];
};

#endif // CODEGEN_ERROR_H
