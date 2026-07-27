#include <stdio.h>
#include <stdlib.h> // malloc(), calloc(), realloc(), free()
#include <string.h> // strdup(), strcmp()

#include "lib.h"

// Create a new dictionary (DictStr)
DictStr* dictstr_new(Void)
{
    DictStr* dict = (DictStr*)calloc(1, sizeof(DictStr));
    if (!dict) {
        PANIC("failed to allocate memory");
    }
    dict->entries = NULL;
    dict->size = 0;
    return dict;
}

// Add a key-value pair to the dictionary
Void dictstr_put(DictStr* dict, const Char* key, const Char* value)
{
    // Ensure the value is not NULL
    if (!value) {
        PANIC("NULL value is not allowed in a dictstr");
        return;
    }

    // Allocate memory for the new entry
    DictStrEntry* entry = (DictStrEntry*)malloc(sizeof(DictStrEntry));
    if (!entry) {
        PANIC("failed to allocate memory");
    }
    entry->key = strdup(key);
    entry->value = strdup(value);

    // Add the new entry to the dictionary
    DictStrEntry** new_entries = (DictStrEntry**)realloc(dict->entries, (dict->size + 1) * sizeof(DictStrEntry*));
    if (!new_entries) {
        PANIC("failed to allocate memory");
    }
    dict->entries = new_entries;
    dict->entries[dict->size++] = entry;
}

// Retrieve a value by its key from the dictionary
Char* dictstr_get(const DictStr* dict, const Char* key)
{

    for (I32 i = 0; i < dict->size; i++) {
        if (strcmp(dict->entries[i]->key, key) == 0) {
            return dict->entries[i]->value;
        }
    }
    return NULL; // key not found
}

Void dictstr_print(const DictStr* dict)
{
    if (dict->size == 0) {
        printf("The dictionary is empty.\n");
        return;
    }

    printf("Dictionary contains %d entries:\n", dict->size);

    for (I32 i = 0; i < dict->size; i++) {
        DictStrEntry* entry = dict->entries[i];
        printf("Key: %s, Value: %s\n", entry->key, entry->value);
    }
}

// Free the memory allocated for the dictionary
Void dictstr_free(DictStr* dict)
{
    for (I32 i = 0; i < dict->size; i++) {
        free(dict->entries[i]->key);
        free(dict->entries[i]->value);
        free(dict->entries[i]);
    }
    free(dict->entries);
    free(dict);
}
