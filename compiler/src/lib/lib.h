#ifndef LIST_H
#define LIST_H

#include <stdarg.h>    // va_list
#include <sys/types.h> // ssize_t

#include "lib.h"

typedef unsigned int U32;
typedef size_t UX;
typedef signed int I32;
typedef ssize_t IX;

typedef char Char;
typedef float F32;
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
Void* list_pop(List* list);
Void* list_get(const List* list, I32 index);
Void list_free(List* list);

// Str
typedef struct {
    Char* char_array_location; // Pointer to the string data
    UX length;                 // Length of the string (excluding null-terminator)
    UX capacity;               // Allocated capacity (including space for null-terminator)
} Str;

// Creation and Destruction
Str* str_new(const Char* value); // Create a new Str from a C-string
Str* str_new_c(const Char c);    // Create a new Str with a single character
Void str_free(Str* str);         // Free the memory associated with the Str

// String Manipulation
Void str_cat_str(Str* dest, const Str* src);                             // Concatenate another Str to this one
Void str_cat(Str* dest, const Char* src);                                // Concatenate a C-string to this one
Void str_cat_c(Str* dest, const Char c);                                 // Concatenate a single character to this one
Void str_insert(Str* dest, UX index, const Char* src);                   // Insert a C-string at a specific position
Void str_remove_range(Str* str, UX start, UX end);                       // Remove a range of characters from the string
Void str_replace(Str* str, const Char* target, const Char* replacement); // Replace all instances of target with replacement

// String Operations
Bool char_cmp(const Char* s1, const Char* s2);
Bool str_cmp(const Str* s1, const Char* s2);              // Compare a Str with a C-string
Bool str_cmp_str(const Str* s1, const Str* s2);           // Compare two Str objects
Char* str_to_char(const Str* str);                        // Convert the Str to a C-string
UX str_find(const Str* str, const Char* target);          // Find the first occurrence of target in the string
Bool str_starts_with(const Str* str, const Char* prefix); // Check if the string starts with a prefix
Bool str_ends_with(const Str* str, const Char* suffix);   // Check if the string ends with a suffix

// Dict
typedef struct {
    Char* key;
    Char* value;
} DictEntry;

typedef struct {
    DictEntry** entries;
    I32 size;
} Dict;
Dict* dict_new(Void);
Void dict_put(Dict* dict, const Char* key, const Char* value);
Char* dict_get(const Dict* dict, const Char* key);
Void dict_print(const Dict* dict);
Void dict_free(Dict* dict);

// io
typedef enum {
    MSG_STEP,
    MSG_INFO,
    MSG_ERROR,
} MsgType;


Void* safe_malloc(UX size);
Void* safe_calloc(UX num, UX size);
Void* safe_realloc(Void* ptr, UX new_size);

typedef struct {
    Char* output;
    I32 return_code;
} CommandResult;

Char* read_file(const Char* filename);
Void write_file(const Char* filename, const Char* outbuffer);
CommandResult sh(const Char* cmd);
Void create_dir(const Char* dir);
void print_v(MsgType msg_type, const Char* text, va_list args);
void print(MsgType msg_type, const Char* text, ...);

// error handling
Void application_panic(const Char* file_path, I32 line, const Char* format, ...);
Void application_assert(const Char* file, I32 line, Bool cond, const Char* message);
Void user_panic(const Char* current_filename, I32 line, const Char* format, ...);

#define USER_PANIC(current_filename, line, ...) user_panic(current_filename, line, __VA_ARGS__)
#define PANIC(...) application_panic(__FILE__, __LINE__, __VA_ARGS__)
#define UNREACHABLE() PANIC("unreachable")
#define ASSERT(cond, message) application_assert(__FILE__, __LINE__, cond, message)

// math
#define MAX(a, b) \
    a > b ? a : b

#define MIN(a, b) \
    a < b ? a : b

#endif
