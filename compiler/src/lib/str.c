#include "lib.h"
#include <stdio.h>
#include <string.h>

Char* str_new(const Char* value)
{
    size_t length = strlen(value);
    Char* str = malloc(length + 1); // +1 pour le caractère nul de fin
    if (!str) {
        PANIC("malloc failed");
    }
    strcpy(str, value);
    return str;
}

Char* str_new_c(const Char c)
{
    Char* str = malloc(2 * sizeof(Char));
    if (!str) {
        PANIC("malloc failed");
    }
    str[0] = c;
    str[1] = '\0';
    return str;
}

/**
 * Concatenate src to dest, and realloc to match the new dest size
 */
Char* str_cat(Char* dest, const Char* src)
{
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);

    Char* new_dest = realloc(dest, dest_len + src_len + 1);
    if (!new_dest) {
        PANIC("realloc failed");
    }

    strcpy(new_dest + dest_len, src);

    return new_dest;
}

Char* str_cat_c(Char* dest, const Char c)
{
    size_t dest_len = strlen(dest);
    Char* new_dest = realloc(dest, dest_len + 2);
    if (!new_dest) {
        PANIC("realloc failed");
    }
    new_dest[dest_len] = c;
    new_dest[dest_len + 1] = '\0';

    return new_dest;
}

B32 str_cmp(const Char* s1, const Char* s2)
{
    if (strcmp(s1, s2) == 0) {
        return true;
    }
    else {
        return false;
    }
}

Void str_free(Char* str)
{
    free(str);
}