#include <codegen.h>
#include <util.h>
#include <ptrarr.h>
#include <codegen_error.h>

#include <malloc.h>

bool codegen_init(Codegen *codegen, const CodegenParams *params){
    (void)params;//unused

    *codegen = (Codegen){0};
    return true;
}

void codegen_cleanup(Codegen *codegen){
    DYN_ARRAY_FOREACH(codegen->errors, err){
        error_free(*err);
    }

    if(codegen->errors){
        free(codegen->errors);
    }
}
