#include "lib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Str* str_new(const Char* value) 
{
    UX length = strlen(value);
    Char* char_array = malloc(length + 1); // +1 pour le caractère nul de fin
    if (!char_array) {
        PANIC("malloc failed");
    }
    strcpy(char_array, value);

    Str* str = malloc(sizeof(Str));
    if (!str) {
        PANIC("malloc failed");
    }
    str->char_array_location = char_array;
    return str;
}

Str* str_new_c(const Char c) 
{
    Char* char_array = malloc(2 * sizeof(Char)); // 2 pour le caractère et le caractère nul de fin
    if (!char_array) {
        PANIC("malloc failed");
    }
    char_array[0] = c;
    char_array[1] = '\0';

    Str* str = malloc(sizeof(Str));
    if (!str) {
        PANIC("malloc failed");
    }
    str->char_array_location = char_array;
    return str;
}

Void str_cat_str(Str* dest, const Str* src)
{
    UX dest_len = strlen(dest->char_array_location);
    UX src_len = strlen(src->char_array_location);

    Char* new_dest = realloc(dest->char_array_location, dest_len + src_len + 1);
    if (!new_dest) {
        PANIC("realloc failed");
    }

    strcpy(new_dest + dest_len, src->char_array_location);
    dest->char_array_location = new_dest;
}

Void str_cat(Str* dest, const Char* src) 
{
    UX dest_len = strlen(dest->char_array_location);
    UX src_len = strlen(src);

    Char* new_dest = realloc(dest->char_array_location, dest_len + src_len + 1);
    if (!new_dest) {
        PANIC("realloc failed");
    }

    strcpy(new_dest + dest_len, src);
    dest->char_array_location = new_dest;
}

Void str_cat_c(Str* dest, const Char c) 
{
    UX dest_len = strlen(dest->char_array_location);
    Char* new_dest = realloc(dest->char_array_location, dest_len + 2);
    if (!new_dest) {
        PANIC("realloc failed");
    }
    new_dest[dest_len] = c;
    new_dest[dest_len + 1] = '\0';

    dest->char_array_location = new_dest;
}

Bool str_cmp(const Str* s1, const Char* s2)
{
    if (strcmp(s1->char_array_location, s2) == 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Bool char_cmp(const Char* s1, const Char* s2)
{
    if (strcmp(s1, s2) == 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Char* str_to_char(const Str* str) 
{
    return str->char_array_location;
}

Void str_free(Str* str) 
{
    free(str->char_array_location);
    free(str);
}