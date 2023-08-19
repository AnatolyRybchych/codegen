#include <codegen.h>

#include <malloc.h>

bool codegen_init(Codegen *codegen, const CodegenParams *params){
    (void)params;//unused

    *codegen = (Codegen){0};
    return true;
}

void codegen_cleanup(Codegen *codegen){
    if(codegen->errors){
        free(codegen->errors);
    }
}
