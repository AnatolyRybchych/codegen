#include <source.h>

#include <stdio.h>
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

    const char *source_path = argv[1];
    Source *source = source_new(source_path);
    assert(source != NULL);

    source_delete(source);

    return 0;
}
