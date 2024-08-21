#ifndef LIST_H
#define LIST_H

#include <stdarg.h>    // va_list
#include <sys/types.h> // ssize_t

#include "lib.h"

// Bool
typedef enum {
    FALSE = 0,
    TRUE = 1
} Bool;

// List
typedef struct {
    void** items; // pointer to an array of pointers, to access an item
    int size;
    int item_size;
} List;
List* list_new(int item_size);
void list_push(List* list, void* item);
void* list_pop(List* list);
void* list_get(const List* list, int index);
void list_free(List* list);

// Str
typedef struct {
    char* char_array_location; // Pointer to the string data
    size_t length;                 // Length of the string (excluding null-terminator)
    size_t capacity;               // Allocated capacity (including space for null-terminator)
} Str;

// Creation and Destruction
Str* str_new(const char* value); // Create a new Str from a C-string
Str* str_new_c(const char c);    // Create a new Str with a single character
void str_free(Str* str);         // Free the memory associated with the Str

// String Manipulation
void str_cat_str(Str* dest, const Str* src);                             // Concatenate another Str to this one
void str_cat(Str* dest, const char* src);                                // Concatenate a C-string to this one
void str_cat_c(Str* dest, const char c);                                 // Concatenate a single character to this one
void str_insert(Str* dest, size_t index, const char* src);                   // Insert a C-string at a specific position
void str_remove_range(Str* str, size_t start, size_t end);                       // Remove a range of characters from the string
void str_replace(Str* str, const char* target, const char* replacement); // Replace all instances of target with replacement

// String Operations
Bool char_cmp(const char* s1, const char* s2);
Bool str_cmp(const Str* s1, const char* s2);              // Compare a Str with a C-string
Bool str_cmp_str(const Str* s1, const Str* s2);           // Compare two Str objects
char* str_to_char(const Str* str);                        // Convert the Str to a C-string
size_t str_find(const Str* str, const char* target);          // Find the first occurrence of target in the string
Bool str_starts_with(const Str* str, const char* prefix); // Check if the string starts with a prefix
Bool str_ends_with(const Str* str, const char* suffix);   // Check if the string ends with a suffix

// Dict
typedef struct {
    char* key;
    char* value;
} DictEntry;

typedef struct {
    DictEntry** entries;
    int size;
} Dict;
Dict* dict_new(void);
void dict_put(Dict* dict, const char* key, const char* value);
char* dict_get(const Dict* dict, const char* key);
void dict_print(const Dict* dict);
void dict_free(Dict* dict);

// io
typedef enum {
    MSG_STEP,
    MSG_INFO,
    MSG_ERROR,
} MsgType;


void* safe_malloc(size_t size);
void* safe_calloc(size_t num, size_t size);
void* safe_realloc(void* ptr, size_t new_size);

typedef struct {
    char* output;
    int return_code;
} CommandResult;

char* read_file(const char* filename);
void write_file(const char* filename, const char* outbuffer);
CommandResult sh(const char* cmd);
void create_dir(const char* dir);
void print_v(MsgType msg_type, const char* text, va_list args);
void print(MsgType msg_type, const char* text, ...);

// error handling
void application_panic(const char* file_path, int line, const char* format, ...);
void application_assert(const char* file, int line, Bool cond, const char* message);
void user_panic(const char* current_filename, int line, const char* format, ...);

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
