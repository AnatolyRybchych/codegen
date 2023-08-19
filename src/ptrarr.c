#include <ptrarr.h>
#include <stdlib.h>


PtrArr *ptrarr_alloc(PtrArr *arr, size_t capacity){
    size_t count = arr ? arr->count : 0;
    PtrArr *result = realloc(arr, sizeof(PtrArr) + sizeof(void*[capacity]));
    if(result != NULL){
        result->capacity = capacity;
        result->count = count;
    }

    return result;
}

PtrArr *ptrarr_push(PtrArr *arr, void *element){
    PtrArr *result = arr;
    if(arr == NULL || arr->count == arr->capacity){
        result = ptrarr_alloc(arr, arr? arr->capacity * 2 + 1 : 4);
        if(result == NULL){
            return NULL;
        }
    }

    result->elements[result->count++] = element;
    return result;
}
