#include <stdio.h>
#include <stdlib.h> // malloc(), calloc(), realloc(), free()
#include <string.h> // strdup(), strcmp()

#include "lib.h"

// Create a new dictionary (Dict)
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

// Add a key-value pair to the dictionary
Void dict_put(Dict* dict, const Char* key, const Char* value)
{
    // Ensure the value is not NULL
    if (!value) {
        PANIC("NULL value is not allowed in a dict");
        return;
    }

    // Allocate memory for the new entry
    DictEntry* entry = (DictEntry*)malloc(sizeof(DictEntry));
    if (!entry) {
        PANIC("failed to allocate memory");
    }
    entry->key = strdup(key);
    entry->value = strdup(value);

    // Add the new entry to the dictionary
    DictEntry** new_entries = (DictEntry**)realloc(dict->entries, (dict->size + 1) * sizeof(DictEntry*));
    if (!new_entries) {
        PANIC("failed to allocate memory");
    }
    dict->entries = new_entries;
    dict->entries[dict->size++] = entry;
}

// Retrieve a value by its key from the dictionary
Char* dict_get(const Dict* dict, const Char* key)
{

    for (I32 i = 0; i < dict->size; i++) {
        if (strcmp(dict->entries[i]->key, key) == 0) {
            return dict->entries[i]->value;
        }
    }
    return NULL; // key not found
}

Void dict_print(const Dict* dict)
{
    if (dict->size == 0) {
        printf("The dictionary is empty.\n");
        return;
    }

    printf("Dictionary contains %d entries:\n", dict->size);

    for (I32 i = 0; i < dict->size; i++) {
        DictEntry* entry = dict->entries[i];
        printf("Key: %s, Value: %s\n", entry->key, entry->value);
    }
}

// Free the memory allocated for the dictionary
Void dict_free(Dict* dict)
{
    for (I32 i = 0; i < dict->size; i++) {
        free(dict->entries[i]->key);
        free(dict->entries[i]->value);
        free(dict->entries[i]);
    }
    // free(dict->entries);
    free(dict);
}
