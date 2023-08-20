#ifndef ARRAY_INT_H
#define ARRAY_INT_H

#include <stdlib.h>

typedef struct ArrInt ArrInt;

struct ArrInt{
    size_t capacity;
    size_t count;
    int elements[];
};

#endif // ARRAY_INT_H
