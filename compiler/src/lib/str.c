#include <stdlib.h> // malloc, realloc, free
#include <string.h> // strcpy, strlen, strcmp, strncpy

#include "lib.h"

// Create a new Str from a C-string
Str* str_new(const Char* value)
{
    UX length = strlen(value);
    Char* char_array = safe_malloc(length + 1); // +1 for the null-terminating character
    strcpy(char_array, value);

    Str* str = safe_malloc(sizeof(Str));
    str->char_array_location = char_array;
    str->length = length;
    str->capacity = length + 1;
    return str;
}

// Create a new Str with a single character
Str* str_new_c(const Char c)
{
    Char* char_array = safe_malloc(2 * sizeof(Char)); // 2 for the character and the null-terminating character
    char_array[0] = c;
    char_array[1] = '\0';

    Str* str = safe_malloc(sizeof(Str));
    str->char_array_location = char_array;
    str->length = 1;
    str->capacity = 2;
    return str;
}

// Concatenate another Str to this one
Void str_cat_str(Str* dest, const Str* src)
{
    UX new_length = dest->length + src->length;

    if (new_length + 1 > dest->capacity) {
        Char* new_dest = safe_realloc(dest->char_array_location, new_length + 1);
        dest->char_array_location = new_dest;
        dest->capacity = new_length + 1;
    }

    strcpy(dest->char_array_location + dest->length, src->char_array_location);
    dest->length = new_length;
}

// Concatenate a C-string to this one
Void str_cat(Str* dest, const Char* src)
{
    UX src_len = strlen(src);
    UX new_length = dest->length + src_len;

    if (new_length + 1 > dest->capacity) {
        Char* new_dest = safe_realloc(dest->char_array_location, new_length + 1);
        dest->char_array_location = new_dest;
        dest->capacity = new_length + 1;
    }

    strcpy(dest->char_array_location + dest->length, src);
    dest->length = new_length;
}

// Concatenate a single character to this one
Void str_cat_c(Str* dest, const Char c)
{
    if (dest->length + 2 > dest->capacity) {
        Char* new_dest = safe_realloc(dest->char_array_location, dest->length + 2);
        dest->char_array_location = new_dest;
        dest->capacity = dest->length + 2;
    }
    dest->char_array_location[dest->length] = c;
    dest->char_array_location[dest->length + 1] = '\0';
    dest->length += 1;
}

// Insert a C-string at a specific position
Void str_insert(Str* dest, UX index, const Char* src)
{
    if (index > dest->length) {
        PANIC("index out of bounds");
    }

    UX src_len = strlen(src);
    UX new_length = dest->length + src_len;

    if (new_length + 1 > dest->capacity) {
        Char* new_dest = safe_realloc(dest->char_array_location, new_length + 1);
        dest->char_array_location = new_dest;
        dest->capacity = new_length + 1;
    }

    memmove(dest->char_array_location + index + src_len, dest->char_array_location + index, dest->length - index + 1);
    memcpy(dest->char_array_location + index, src, src_len);
    dest->length = new_length;
}

// Remove a range of characters from the string
Void str_remove_range(Str* str, UX start, UX end)
{
    if (start > str->length || end > str->length || start > end) {
        PANIC("invalid range");
    }

    memmove(str->char_array_location + start, str->char_array_location + end, str->length - end + 1);
    str->length -= (end - start);
}

// Replace all instances of target with replacement
Void str_replace(Str* str, const Char* target, const Char* replacement)
{
    UX target_len = strlen(target);
    // UX replacement_len = strlen(replacement);
    Char* pos;

    while ((pos = strstr(str->char_array_location, target)) != NULL) {
        UX pos_index = pos - str->char_array_location;

        // Remove the target
        str_remove_range(str, pos_index, pos_index + target_len);

        // Insert the replacement
        str_insert(str, pos_index, replacement);
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

// Compare a Str with a C-string
Bool str_cmp(const Str* s1, const Char* s2)
{
    return strcmp(s1->char_array_location, s2) == 0 ? TRUE : FALSE;
}

// Compare two Str objects
Bool str_cmp_str(const Str* s1, const Str* s2)
{
    return strcmp(s1->char_array_location, s2->char_array_location) == 0 ? TRUE : FALSE;
}

// Convert the Str to a C-string
Char* str_to_char(const Str* str)
{
    return str->char_array_location;
}

// Find the first occurrence of target in the string
UX str_find(const Str* str, const Char* target)
{
    Char* pos = strstr(str->char_array_location, target);
    if (pos) {
        return pos - str->char_array_location;
    }
    return -1; // not found
}

// Check if the string starts with a prefix
Bool str_starts_with(const Str* str, const Char* prefix)
{
    UX prefix_len = strlen(prefix);
    if (prefix_len > str->length) {
        return FALSE;
    }
    return strncmp(str->char_array_location, prefix, prefix_len) == 0 ? TRUE : FALSE;
}

// Check if the string ends with a suffix
Bool str_ends_with(const Str* str, const Char* suffix)
{
    UX suffix_len = strlen(suffix);
    if (suffix_len > str->length) {
        return FALSE;
    }
    return strcmp(str->char_array_location + str->length - suffix_len, suffix) == 0 ? TRUE : FALSE;
}

// Free the memory associated with the Str
Void str_free(Str* str)
{
    free(str->char_array_location);
    free(str);
}
