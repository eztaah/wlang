#include "lib.h"
#include <stdio.h>
#include <string.h>

B32 strings_are_equals(const Char* s1, const Char* s2)
{
    if (strcmp(s1, s2) == 0) {
        return true;
    }
    else {
        return false;
    }
}

Char* init_empty_string(Void)
{
    Char* str = malloc(1);
    if (!str) {
        PANIC("malloc failed");
    }
    str[0] = '\0';
    return str;
}

Char* init_string_with_value(const Char* value)
{
    size_t length = strlen(value);
    Char* str = malloc(length + 1); // +1 pour le caractère nul de fin
    if (!str) {
        PANIC("malloc failed");
    }
    strcpy(str, value);
    return str;
}

/**
 * Concatenate src to dest, and realloc to match the new dest size
 */

Char* strcat_improved(Char* dest, const Char* src)
{
    // Vérifiez si les chaînes d'entrée ne sont pas NULL
    if (!dest || !src) {
        PANIC("dest or scr are NULL");
    }

    // Calculer la longueur de la chaîne de destination
    size_t dest_len = strlen(dest);
    // Calculer la longueur de la chaîne source
    size_t src_len = strlen(src);

    // Allouer de la mémoire pour la nouvelle chaîne combinée
    // +1 pour le caractère nul de fin
    Char* new_dest = realloc(dest, dest_len + src_len + 1);
    if (!new_dest) {
        PANIC("realloc failed");
    }

    // Copier la chaîne source à la fin de la chaîne destination
    strcpy(new_dest + dest_len, src);

    return new_dest;
}
