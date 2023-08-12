#include <eval.h>
#include <source.h>

#include <string.h>
#include <errno.h>

int eval(Str file_path, FILE *output){
    Source *source = source_new(file_path);
    if(source == NULL){
        fprintf(
            stderr,
            "ERROR: Could not read a source '%.*s': %s",
            (int)(file_path.end - file_path.beg),
            file_path.beg,
            strerror(errno)
        );

        return 1;
    }

    (void)output;

    source_delete(source);

    return 0;
}