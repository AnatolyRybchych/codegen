#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdbool.h>

typedef struct Codegen Codegen;
typedef struct CodegenParams CodegenParams;

struct CodegenParams{
    int _;
};

struct Codegen{
    struct PtrArr *errors;
};

bool codegen_init(Codegen *codegen, const CodegenParams *params);
void codegen_cleanup(Codegen *codegen);

#endif // CODEGEN_H
