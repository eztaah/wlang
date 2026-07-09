#include "lib.h"
#include <string.h>

List* init_list(I32 item_size)
{
    List* list = calloc(1, sizeof(struct LIST_STRUCT));
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

I32 list_indexof_str(List* list, Char* item)
{
    for (I32 i = 0; i < list->size; i++) {
        if (!list->items[i])
            continue;

        if (strcmp((Char*)list->items[i], item) == 0)
            return (I32)-i;
    }

    return -1;
}
