#ifndef LIST_H
#define LIST_H
#include "lib.h"
#include <stdlib.h>

typedef unsigned char C8;
typedef unsigned int U32;
typedef int I32;
typedef int B32;
typedef float F32;
typedef char Char;
typedef void Void;

#define true 1
#define false 0

// List
typedef struct {
    Void** items; // pointeur vers un array de pointeurs, pour acceder à un item
    I32 size;
    I32 item_size;
} List;
List* list_new(I32 item_size);
Void list_push(List* list, Void* item);
Void* list_get(const List* list, I32 index);
Void list_free(List* list);

// Str
Char* str_new(const Char* value);
Char* str_new_c(const Char c);
Char* str_cat(Char* dest, const Char* src);
Char* str_cat_c(Char* dest, const Char c);
B32 str_cmp(const Char* s1, const Char* s2);
Void str_free(Char* str);

// io
Char* read_file(const Char* filename);
Void write_file(const Char* filename, Char* outbuffer);
Char* sh(const Char* cmd);

// error handling
Void application_panic(const Char* file_path, I32 line, const Char* format, ...);
Void application_assert(const Char* file, I32 line, B32 cond, const Char* message);
#define PANIC(...) application_panic(__FILE__, __LINE__, __VA_ARGS__)
#define UNREACHABLE() PANIC("unreachable")
#define ASSERT(cond, message) application_assert(__FILE__, __LINE__, cond, message)

#endif
