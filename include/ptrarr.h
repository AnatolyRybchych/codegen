#ifndef PTRARR_H
#define PTRARR_H

#include <stddef.h>

typedef struct PtrArr PtrArr;

PtrArr *ptrarr_alloc(PtrArr *arr, size_t capacity);
PtrArr *ptrarr_push(PtrArr *arr, void *element);

struct PtrArr{
    size_t capacity;
    size_t count;
    void *elements[];
};

#endif // PTRARR_H
