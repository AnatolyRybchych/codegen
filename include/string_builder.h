#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <string.h>
#include <str.h>
#include <ptrarr.h>

#include <stdio.h>

typedef struct StringBuilder StringBuilder;

void sb_init(StringBuilder *sb);
bool sb_out_of_memory(StringBuilder *sb);

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
    bool out_of_memory;
};

#endif // STRING_BUILDER_H
