#include "lib.h"
#include <stdlib.h>
#include <string.h>

Map* map_new(Void) 
{
    Map* map = (Map*)calloc(1, sizeof(Map));
    if (!map) {
        PANIC("failed to allocate memory");
    }
    map->entries = NULL;
    map->size = 0;
    return map;
}

Void map_put(Map* map, const Char* key, I32 value) 
{
    MapEntry* entry = (MapEntry*)malloc(sizeof(MapEntry));
    if (!entry) {
        PANIC("failed to allocate memory");
    }
    entry->key = strdup(key);
    if (!entry->key) {
        PANIC("failed to allocate memory");
    }
    entry->value = value;

    MapEntry** new_entries = (MapEntry**)realloc(map->entries, (map->size + 1) * sizeof(MapEntry*));
    if (!new_entries) {
        PANIC("failed to allocate memory");
    }
    map->entries = new_entries;
    map->entries[map->size++] = entry;
}

I32 map_get(Map* map, const Char* key) 
{
    for (I32 i = 0; i < map->size; i++) {
        if (strcmp(map->entries[i]->key, key) == 0) {
            return map->entries[i]->value;
        }
    }
    return -100;   // key not found
}

Void map_free(Map* map) 
{
    for (I32 i = 0; i < map->size; i++) {
        free(map->entries[i]->key); 
        free(map->entries[i]);     
    }
    free(map->entries);
    free(map);   
}