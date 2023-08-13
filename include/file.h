#ifndef FILE_H
#define FILE_H

#include <str.h>
#include <stddef.h>
#include <stdio.h>

typedef struct FileData FileData;

FileData *filedata_from_stream(FILE *stream, Str as_path);
FileData *filedata_from_file(Str path);
void filedata_free(FileData *data);

struct FileData{
    size_t path_size;
    char *real_path;

    size_t size;
    char data[];
};

#endif // FILE_H
