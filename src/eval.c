#include <eval.h>
#include <file.h>
#include <expr.h>
#include <util.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

int eval(Str file_path, FILE *output){
    (void)output;

    FileData *file_data = filedata_from_file(file_path);
    Str file_str = STR(file_data->data, file_data->data + file_data->size);

    ExprArray *expressions = parse_expressions(file_str);
    DYN_ARRAY_FOREACH(expressions, expr){
        printf(STR_FMT ": " STR_FMT "\n\n", STR_ARG(expr->name), STR_ARG(expr->body));
    }

    filedata_free(file_data);

    return 0;
}