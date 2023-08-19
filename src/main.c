#include <eval.h>

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

    EvalResult result = eval_file(STR(argv[1], argv[1] + strlen(argv[1])));
    if(result.status == EVAL_SUCCESS){
        printf("%s\n", result.string->elements);
    }
    else{
        printf("evalueation FAILED\n");
    }
    eval_result_ceanup(&result);

    return result.status != EVAL_SUCCESS;
}
