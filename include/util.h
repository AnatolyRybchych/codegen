#ifndef UTIL_H
#define UTIL_H

#define ARRLEN(ARR) ((sizeof(ARR)) / (sizeof(*ARR)))
#define DYN_ARRLEN(ARR) ((ARR) ? (ARR)->count: 0)

#define DYN_ARRAY_FOREACH(ARRAY, ELEMENT) \
    if(ARRAY) \
    for(typeof(*(ARRAY)->elements) *ELEMENT = (ARRAY)->elements; \
        ELEMENT != (ARRAY)->elements + (ARRAY)->count; \
        ELEMENT++)

#define DYN_ARRAY_REVERSE_FOREACH(ARRAY, ELEMENT) \
    if(ARRAY) \
    for(typeof(*(ARRAY)->elements) *ELEMENT = (ARRAY)->elements + (ARRAY)->count - 1; \
        ELEMENT != (ARRAY)->elements - 1; \
        ELEMENT--)

#define ARRAY_FOREACH(ARRAY, ELEMENT) \
    for(typeof(*(ARRAY)) *ELEMENT = (ARRAY); \
        ELEMENT != (ARRAY) + ARRLEN(ARRAY); \
        ELEMENT++)

#endif // UTIL_H
