#ifndef LIST_H
#define LIST_H

#include "lib.h"
#include <sys/types.h> // for ssize_t

typedef unsigned int U32;
typedef size_t UX;
typedef int I32;
typedef ssize_t IX;
typedef float F32;
typedef char Char;
typedef void Void;

// Bool
typedef enum {
    FALSE = 0,
    TRUE = 1
} Bool;

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
typedef struct {
    Char* char_array_location;
} Str;
Str* str_new(const Char* value);
Str* str_new_c(const Char c);
Void str_cat_str(Str* dest, const Str* src);
Void str_cat(Str* dest, const Char* src);
Void str_cat_c(Str* dest, const Char c);
Bool str_cmp(const Str* s1, const Char* s2);
Char* str_to_char(const Str* str);
Void str_free(Str* str);
Bool char_cmp(const Char* s1, const Char* s2);

// Map
typedef struct {
    Char* key;
    I32 value;
} MapEntry;
typedef struct {
    MapEntry** entries; 
    I32 size; 
} Map;
Map* map_new(Void);
Void map_put(Map* map, const Char* key, I32 value);
I32 map_get(Map* map, const Char* key);
Void map_free(Map* map);

// io
Char* read_file(const Char* filename);
Void write_file(const Char* filename, Char* outbuffer);
Char* sh(const Char* cmd);

// error handling
Void application_panic(const Char* file_path, I32 line, const Char* format, ...);
Void application_assert(const Char* file, I32 line, Bool cond, const Char* message);
#define PANIC(...) application_panic(__FILE__, __LINE__, __VA_ARGS__)
#define UNREACHABLE() PANIC("unreachable")
#define ASSERT(cond, message) application_assert(__FILE__, __LINE__, cond, message)

// math
#define MAX(a, b) \
    a > b ? a : b

#define MIN(a, b) \
    a < b ? a : b

#endif
