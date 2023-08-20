#ifndef ARRAY_DOUBLE_H
#define ARRAY_DOUBLE_H

#include <stdlib.h>

typedef struct ArrDouble ArrDouble;

struct ArrDouble{
    size_t capacity;
    size_t count;
    double elements[];
};

#endif // ARRAY_DOUBLE_H
