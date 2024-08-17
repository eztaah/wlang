#include <stdlib.h> // malloc()

#include "lib.h"

List* list_new(I32 item_size)
{
    List* list = calloc(1, sizeof(List));
    list->size = 0;
    list->item_size = item_size;
    list->items = 0;

    return list;
}

Void list_push(List* list, Void* item)
{
    list->size += 1;

    if (!list->items)
        list->items = calloc(1, list->item_size);
    else
        list->items = realloc(list->items, (list->size * list->item_size));

    list->items[list->size - 1] = item;
}

Void* list_pop(List* list)
{
    if (list->size == 0) {
        PANIC("pop from empty list");
        return NULL;
    }

    // Get the last item in the list
    Void* item = list->items[list->size - 1];

    // Reduce the size of the list
    list->size -= 1;

    if (list->size == 0) {
        free(list->items);
        list->items = NULL;
    } else {
        // Reallocate the memory for the items array to match the new size
        list->items = realloc(list->items, list->size * list->item_size);
    }

    return item;
}


Void* list_get(const List* list, I32 index)
{
    if (index < 0 || index >= list->size) {
        PANIC("index out of bounds");
        return NULL;
    }
    return list->items[index];
}

Void list_free(List* list)
{
    for (I32 i = 0; i < list->size; ++i) {
        free(list_get(list, i)); // free each item
    }
    free(list->items); // free the items array
    free(list);        // free the list structure itself

    // TOFIX: If the list contains an object that contains a list, these lists will not be freed
}