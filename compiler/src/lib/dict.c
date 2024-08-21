#include <stdio.h>
#include <stdlib.h> // malloc(), calloc(), realloc(), free()
#include <string.h> // strdup(), strcmp()

#include "lib.h"

// Create a new dictionary (Dict)
Dict* dict_new(void)
{
    Dict* dict = (Dict*)safe_calloc(1, sizeof(Dict));
    dict->entries = NULL;
    dict->size = 0;
    return dict;
}

// Add a key-value pair to the dictionary
void dict_put(Dict* dict, const char* key, const char* value)
{
    // Ensure the value is not NULL
    if (!value) {
        PANIC("NULL value is not allowed in a dict");
        return;
    }

    // Allocate memory for the new entry
    DictEntry* entry = (DictEntry*)safe_malloc(sizeof(DictEntry));
    entry->key = strdup(key);
    entry->value = strdup(value);

    // Add the new entry to the dictionary
    DictEntry** new_entries = (DictEntry**)safe_realloc(dict->entries, (dict->size + 1) * sizeof(DictEntry*));
    dict->entries = new_entries;
    dict->entries[dict->size++] = entry;
}

// Retrieve a value by its key from the dictionary
char* dict_get(const Dict* dict, const char* key)
{

    for (int i = 0; i < dict->size; i++) {
        if (strcmp(dict->entries[i]->key, key) == 0) {
            return dict->entries[i]->value;
        }
    }
    return NULL; // key not found
}

void dict_print(const Dict* dict)
{
    if (dict->size == 0) {
        printf("The dictionary is empty.\n");
        return;
    }

    printf("Dictionary contains %d entries:\n", dict->size);

    for (int i = 0; i < dict->size; i++) {
        DictEntry* entry = dict->entries[i];
        printf("Key: %s, Value: %s\n", entry->key, entry->value);
    }
}

// Free the memory allocated for the dictionary
void dict_free(Dict* dict)
{
    for (int i = 0; i < dict->size; i++) {
        free(dict->entries[i]->key);
        free(dict->entries[i]->value);
        free(dict->entries[i]);
    }
    // free(dict->entries);
    free(dict);
}
