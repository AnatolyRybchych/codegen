#include <file.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

static long stream_size(FILE *stream);

FileData *filedata_from_stream(FILE *stream, Str as_path){
    long file_size = stream_size(stream);
    if(file_size <= 0){
        file_size = 0;
    }

    FileData *result = malloc(sizeof(FileData) + sizeof(char[file_size + 1]));
    if(result == NULL){
        return NULL;
    }

    fread(result->data, 1, file_size, stream);
    result->size = file_size;
    result->data[file_size] = '\0';

    result->path_size = as_path.end - as_path.beg;
    result->real_path = malloc(sizeof(char[result->path_size]));

    if(result->real_path == NULL){
        free(result);
        return NULL;
    }
    else{
        memcpy(result->real_path, as_path.beg, result->path_size);
        result->real_path[result->path_size] = '\0';
        return result;
    }
}

FileData *filedata_from_file(Str path){
    char filename[FILENAME_MAX];
    memcpy(filename, path.beg, path.end - path.beg);
    filename[path.end - path.beg] = '\0';

    FILE *file = fopen(filename, "r");
    if(file){
        return filedata_from_stream(file, path);
    }
    else{
        return NULL;
    }
}

void filedata_free(FileData *data){
    assert(data != NULL);
    
    if(data->real_path){
        free(data->real_path);
    }

    free(data);
}

static long stream_size(FILE *stream){
    if(stream == NULL){
        return -1;
    }

    long prev = ftell(stream);
    if(prev < 0){
        prev = 0;
    }

    if(fseek(stream, 0, SEEK_END) < 0){
        return -1;
    }

    long result = ftell(stream);
    fseek(stream, prev, SEEK_SET);

    return result;
}
