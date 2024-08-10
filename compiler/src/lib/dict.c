#include "lib.h"
#include <stdlib.h>
#include <string.h>

Dict* dict_new(Void)
{
    Dict* dict = (Dict*)calloc(1, sizeof(Dict));
    if (!dict) {
        PANIC("failed to allocate memory");
    }
    dict->entries = NULL;
    dict->size = 0;
    return dict;
}

Void dict_put(Dict* dict, const Char* key, I32 value)
{
    DictEntry* entry = (DictEntry*)malloc(sizeof(DictEntry));
    if (!entry) {
        PANIC("failed to allocate memory");
    }
    entry->key = strdup(key);
    if (!entry->key) {
        PANIC("failed to allocate memory");
    }
    entry->value = value;

    DictEntry** new_entries = (DictEntry**)realloc(dict->entries, (dict->size + 1) * sizeof(DictEntry*));
    if (!new_entries) {
        PANIC("failed to allocate memory");
    }
    dict->entries = new_entries;
    dict->entries[dict->size++] = entry;
}

I32 dict_get(Dict* dict, const Char* key)
{
    for (I32 i = 0; i < dict->size; i++) {
        if (strcmp(dict->entries[i]->key, key) == 0) {
            return dict->entries[i]->value;
        }
    }
    return -100; // key not found
}

Void dict_free(Dict* dict)
{
    for (I32 i = 0; i < dict->size; i++) {
        free(dict->entries[i]->key);
        free(dict->entries[i]);
    }
    free(dict->entries);
    free(dict);
}