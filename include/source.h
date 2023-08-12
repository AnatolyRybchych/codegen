#ifndef SOURCE_H
#define SOURCE_H

#include <stddef.h>

typedef struct Source Source;

struct Source{
    char *path;
    char *dir;
    char *filename;

    size_t len;
    char data[];
};

Source *source_new(const char *path);
void source_delete(Source *source);

#endif // SOURCE_H
