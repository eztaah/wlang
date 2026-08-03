#include <stdlib.h> // malloc, realloc, free
#include <string.h> // strcpy, strlen, strcmp, strncpy

#include "lib.h"

// create a new Str from a C-string
Str* str_new(const char* value)
{
    size_t length = strlen(value);
    char* char_array = safe_malloc(length + 1); // +1 for the null-terminating character
    strcpy(char_array, value);

    Str* str = safe_malloc(sizeof(Str));
    str->char_array_location = char_array;
    str->length = length;
    str->capacity = length + 1;
    return str;
}

// create a new Str with a single character
Str* str_new_c(const char c)
{
    char* char_array = safe_malloc(2 * sizeof(char)); // 2 for the character and the null-terminating character
    char_array[0] = c;
    char_array[1] = '\0';

    Str* str = safe_malloc(sizeof(Str));
    str->char_array_location = char_array;
    str->length = 1;
    str->capacity = 2;
    return str;
}

// concatenate another Str to this one
void str_cat_str(Str* dest, const Str* src)
{
    size_t new_length = dest->length + src->length;

    if (new_length + 1 > dest->capacity) {
        char* new_dest = safe_realloc(dest->char_array_location, new_length + 1);
        dest->char_array_location = new_dest;
        dest->capacity = new_length + 1;
    }

    strcpy(dest->char_array_location + dest->length, src->char_array_location);
    dest->length = new_length;
}

// concatenate a C-string to this one
void str_cat(Str* dest, const char* src)
{
    size_t src_len = strlen(src);
    size_t new_length = dest->length + src_len;

    if (new_length + 1 > dest->capacity) {
        char* new_dest = safe_realloc(dest->char_array_location, new_length + 1);
        dest->char_array_location = new_dest;
        dest->capacity = new_length + 1;
    }

    strcpy(dest->char_array_location + dest->length, src);
    dest->length = new_length;
}

// concatenate a single character to this one
void str_cat_c(Str* dest, const char c)
{
    if (dest->length + 2 > dest->capacity) {
        char* new_dest = safe_realloc(dest->char_array_location, dest->length + 2);
        dest->char_array_location = new_dest;
        dest->capacity = dest->length + 2;
    }
    dest->char_array_location[dest->length] = c;
    dest->char_array_location[dest->length + 1] = '\0';
    dest->length += 1;
}

// insert a C-string at a specific position
void str_insert(Str* dest, size_t index, const char* src)
{
    if (index > dest->length) {
        PANIC("index out of bounds");
    }

    size_t src_len = strlen(src);
    size_t new_length = dest->length + src_len;

    if (new_length + 1 > dest->capacity) {
        char* new_dest = safe_realloc(dest->char_array_location, new_length + 1);
        dest->char_array_location = new_dest;
        dest->capacity = new_length + 1;
    }

    memmove(dest->char_array_location + index + src_len, dest->char_array_location + index, dest->length - index + 1);
    memcpy(dest->char_array_location + index, src, src_len);
    dest->length = new_length;
}

// remove a range of characters from the string
void str_remove_range(Str* str, size_t start, size_t end)
{
    if (start > str->length || end > str->length || start > end) {
        PANIC("invalid range");
    }

    memmove(str->char_array_location + start, str->char_array_location + end, str->length - end + 1);
    str->length -= (end - start);
}

// replace all instances of target with replacement
void str_replace(Str* str, const char* target, const char* replacement)
{
    size_t target_len = strlen(target);
    char* pos;

    while ((pos = strstr(str->char_array_location, target)) != NULL) {
        size_t pos_index = pos - str->char_array_location;

        str_remove_range(str, pos_index, pos_index + target_len);
        str_insert(str, pos_index, replacement);
    }
}

// compare two C-strings
Bool char_cmp(const char* s1, const char* s2)
{
    return strcmp(s1, s2) == 0 ? TRUE : FALSE;
}

// compare a Str with a C-string
Bool str_cmp(const Str* s1, const char* s2)
{
    return strcmp(s1->char_array_location, s2) == 0 ? TRUE : FALSE;
}

// compare two Str objects
Bool str_cmp_str(const Str* s1, const Str* s2)
{
    return strcmp(s1->char_array_location, s2->char_array_location) == 0 ? TRUE : FALSE;
}

// convert the Str to a C-string
char* str_to_char(const Str* str)
{
    return str->char_array_location;
}

// find the first occurrence of target in the string
size_t str_find(const Str* str, const char* target)
{
    char* pos = strstr(str->char_array_location, target);
    return pos ? pos - str->char_array_location : -1; // return -1 if not found
}

// check if the string starts with a prefix
Bool str_starts_with(const Str* str, const char* prefix)
{
    size_t prefix_len = strlen(prefix);
    if (prefix_len > str->length) {
        return FALSE;
    }
    return strncmp(str->char_array_location, prefix, prefix_len) == 0 ? TRUE : FALSE;
}

// check if the string ends with a suffix
Bool str_ends_with(const Str* str, const char* suffix)
{
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str->length) {
        return FALSE;
    }
    return strcmp(str->char_array_location + str->length - suffix_len, suffix) == 0 ? TRUE : FALSE;
}

// free the memory associated with the Str
void str_free(Str* str)
{
    free(str->char_array_location);
    free(str);
}
