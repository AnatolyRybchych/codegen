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

    return eval((Str){.beg = argv[1], .end = argv[1] + strlen(argv[1])}, stdout);
}
