#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <string.h>
#include <str.h>
#include <ptrarr.h>

#include <stdio.h>

typedef struct StringBuilder StringBuilder;
typedef unsigned StringBuilderError;
enum StringBuilderError{
    STRING_BUILDER_SUCCESS,
    STRING_BUILDER_OUT_OF_MEMORY,
};

void sb_init(StringBuilder *sb);
StringBuilderError sb_error(StringBuilder *sb);

//result should be cleaned up with free()
//can return NULL if out of memmory
String *sb_build(StringBuilder *sb);
void sb_write(StringBuilder *sb, FILE *stream);
void sb_cleanup(StringBuilder *sb);

void sb_fmt(StringBuilder *sb, const char *fmt, ...);
void sb_str(StringBuilder *sb, Str str);
void sb_cstr(StringBuilder *sb, char *cstr);

struct StringBuilder{
    PtrArr *strings;
    StringBuilderError error;
};

#endif // STRING_BUILDER_H
