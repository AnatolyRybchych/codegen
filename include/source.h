#ifndef SOURCE_H
#define SOURCE_H

#include <expr.h>

#include <stddef.h>

typedef struct Source Source;

struct Source{
    char *path;
    char *dir;
    char *filename;

    ExprArray *expressions;

    size_t len;
    char data[];
};

Source *source_new(Str path);
void source_delete(Source *source);

#endif // SOURCE_H