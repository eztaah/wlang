#include <stdlib.h> // malloc()

#include "lib.h"

List* list_new(int item_size)
{
    List* list = safe_calloc(1, sizeof(List));
    list->size = 0;
    list->item_size = item_size;
    list->items = 0;

    return list;
}

void list_push(List* list, void* item)
{
    list->size += 1;

    if (!list->items)
        list->items = safe_calloc(1, list->item_size);
    else
        list->items = safe_realloc(list->items, (list->size * list->item_size));

    list->items[list->size - 1] = item;
}

void* list_pop(List* list)
{
    if (list->size == 0) {
        PANIC("pop from empty list");
        return NULL;
    }

    // Get the last item in the list
    void* item = list->items[list->size - 1];

    // Reduce the size of the list
    list->size -= 1;

    if (list->size == 0) {
        free(list->items);
        list->items = NULL;
    }
    else {
        // Reallocate the memory for the items array to match the new size
        list->items = safe_realloc(list->items, list->size * list->item_size);
    }

    return item;
}

void* list_get(const List* list, int index)
{
    if (index < 0 || index >= list->size) {
        PANIC("index out of bounds");
        return NULL;
    }
    return list->items[index];
}

void list_free(List* list)
{
    for (int i = 0; i < list->size; ++i) {
        free(list_get(list, i)); // free each item
    }
    free(list->items); // free the items array
    free(list);        // free the list structure itself

    // TOFIX: If the list contains an object that contains a list, these lists will not be freed
}