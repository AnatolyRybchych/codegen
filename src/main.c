#include <eval.h>
#include <codegen.h>
#include <codegen_error.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(
            stderr, 
            "ERROR: missing required argument <file>\n"
            "USAGE:\n    %s <file>",
            argv[0]
        );

        exit(1);
    }

    CodegenParams default_params = {0};
    Codegen codegen;
    codegen_init(&codegen, &default_params);

    String *result = eval_file(&codegen, STR(argv[1], argv[1] + strlen(argv[1])));
    if(result){
        printf("%s\n", result->elements);
        free(result);
    }
    else{
        printf("evalueation FAILED\n");
        print_errors(&codegen);
    }

    codegen_cleanup(&codegen);

    return result == NULL;
}
