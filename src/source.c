#include <source.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRLEN(ARR) ((sizeof(ARR)) / (sizeof(*ARR)))

Source *source_new(const char *path){
    FILE *file = fopen(path, "r");
    assert(file != NULL);

    int ret = fseek(file, 0, SEEK_END);
    assert(ret != -1);

    long filesize = ftell(file);
    assert(filesize != -1);

    ret = fseek(file, 0, SEEK_SET);
    assert(ret != -1);

    Source *result = malloc(sizeof(Source) + sizeof(char[filesize + 1]));
    assert(result != NULL);
    memset(result, 0, sizeof(sizeof(Source)));

    result->len = filesize;

    fread(result->data, 1, filesize, file);
    result->data[filesize] = 0;

    size_t path_len = strlen(path);
    result->filename = malloc(path_len + 1);
    assert(result->filename != NULL);
    strcpy(result->filename, path);

    return result;
}

void source_delete(Source *source){
    void *to_free[] = {
        source->path,
        source->dir,
        source->filename,
        source,
    };

    for(size_t i = 0; i < ARRLEN(to_free); i++){
        if(to_free[i] != NULL){
            free(to_free[i]);
        }
    }
}
