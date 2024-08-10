#ifndef LIST_H
#define LIST_H

#include "lib.h"
#include <stdarg.h>    // va_list
#include <sys/types.h> // ssize_t

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
    Void** items; // pointer to an array of pointers, to access an item
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

// Dict
typedef struct {
    Char* key;
    I32 value;
} DictEntry;
typedef struct {
    DictEntry** entries;
    I32 size;
} Dict;
Dict* dict_new(Void);
Void dict_put(Dict* dict, const Char* key, I32 value);
I32 dict_get(const Dict* dict, const Char* key);
Void dict_free(Dict* dict);

// io
typedef enum {
    MSG_STEP,
    MSG_INFO,
    MSG_ERROR,
} MsgType;

Char* read_file(const Char* filename);
Void write_file(const Char* filename, Char* outbuffer);
Char* sh(const Char* cmd);
Void create_dir(const Char* dir);
void print_v(MsgType msg_type, const char* text, va_list args);
void print(MsgType msg_type, const char* text, ...);

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
