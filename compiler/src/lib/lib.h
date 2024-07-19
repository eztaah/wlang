#ifndef LIST_H
#define LIST_H
#include "../global.h"
#include <stdlib.h>

// Lists
typedef struct LIST_STRUCT {
    Void** items;
    I32 size;
    I32 item_size;
} List;

List* init_list(I32 item_size);
Void list_push(List* list, Void* item);
I32 list_indexof_str(List* list, Char* item);


// String
B32 strings_are_equals(const Char* s1, const Char* s2); 
Char* init_empty_string(Void);
Char* init_string_with_value(const Char* value);
Char* strcat_improved(Char* dest, const Char* src);

#endif
