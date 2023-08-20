#ifndef ARRAY_FLOAT_H
#define ARRAY_FLOAT_H

#include <stdlib.h>

typedef struct ArrFloat ArrFloat;

struct ArrFloat{
    size_t capacity;
    size_t count;
    float elements[];
};

#endif // ARRAY_FLOAT_H
