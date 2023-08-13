#ifndef UTIL_H
#define UTIL_H

#define ARRLEN(ARR) ((sizeof(ARR)) / (sizeof(*ARR)))

#define DYN_ARRAY_FOREACH(ARRAY, ELEMENT) \
    if(ARRAY) \
    for(typeof(*(ARRAY)->elements) *ELEMENT = (ARRAY)->elements; \
        ELEMENT != (ARRAY)->elements + (ARRAY)->count; \
        ELEMENT++)

#endif // UTIL_H
