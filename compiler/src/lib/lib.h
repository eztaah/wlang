#ifndef LIST_H
#define LIST_H

#include <stdarg.h>    // va_list
#include <sys/types.h> // ssize_t

#include "lib.h"

typedef enum {
    FALSE = 0,
    TRUE = 1
} Bool;

// List
typedef struct {
    void** items;      // pointer to an array of pointers, to access an item
    int size;          // current number of items in the list
    int item_size;     // size of each item
} List;

List* list_new(int item_size);                       // create a new list with the specified item size
void list_push(List* list, void* item);              // add an item to the end of the list
void* list_pop(List* list);                          // remove and return the last item from the list
void* list_get(const List* list, int index);         // get the item at the specified index
void list_free(List* list);                          // free the memory associated with the list

// Str 
typedef struct {
    char* char_array_location; // pointer to the string data
    size_t length;             // length of the string (excluding null-terminator)
    size_t capacity;           // allocated capacity (including space for null-terminator)
} Str;

// creation and destruction of Str
Str* str_new(const char* value);                     // create a new Str from a C-string
Str* str_new_c(const char c);                        // create a new Str with a single character
void str_free(Str* str);                             // free the memory associated with the Str

// string manipulation functions
void str_cat_str(Str* dest, const Str* src);         // concatenate another Str to this one
void str_cat(Str* dest, const char* src);            // concatenate a C-string to this one
void str_cat_c(Str* dest, const char c);             // concatenate a single character to this one
void str_insert(Str* dest, size_t index, const char* src); // insert a C-string at a specific position
void str_remove_range(Str* str, size_t start, size_t end); // remove a range of characters from the string
void str_replace(Str* str, const char* target, const char* replacement); // replace all instances of target with replacement

// string operations
Bool char_cmp(const char* s1, const char* s2);       // compare two C-strings
Bool str_cmp(const Str* s1, const char* s2);         // compare a Str with a C-string
Bool str_cmp_str(const Str* s1, const Str* s2);      // compare two Str objects
char* str_to_char(const Str* str);                   // convert the Str to a C-string
size_t str_find(const Str* str, const char* target); // find the first occurrence of target in the string
Bool str_starts_with(const Str* str, const char* prefix); // check if the string starts with a prefix
Bool str_ends_with(const Str* str, const char* suffix);   // check if the string ends with a suffix

// Dict
typedef struct {
    char* key;               // key for the dictionary entry
    char* value;             // value associated with the key
} DictEntry;

typedef struct {
    DictEntry** entries;     // array of pointers to dictionary entries
    int size;                // current number of entries
} Dict;

Dict* dict_new(void);                          // create a new dictionary
void dict_put(Dict* dict, const char* key, const char* value); // add a key-value pair to the dictionary
char* dict_get(const Dict* dict, const char* key);             // retrieve a value by its key
void dict_print(const Dict* dict);            // print all key-value pairs in the dictionary
void dict_free(Dict* dict);                   // free the memory associated with the dictionary

// memory allocation 
void* safe_malloc(size_t size);                // allocate memory and handle allocation failure
void* safe_calloc(size_t num, size_t size);    // allocate and zero-initialize memory, handle failure
void* safe_realloc(void* ptr, size_t new_size); // reallocate memory and handle allocation failure

// file and directory operations
typedef struct {
    char* output;           // output of the command
    int return_code;        // return code of the command
} CommandResult;

char* read_file(const char* filename);                     // read the entire content of a file
void write_file(const char* filename, const char* outbuffer); // write content to a file
CommandResult sh(const char* cmd);                         // execute a shell command and capture the result
void create_dir(const char* dir);                          // create a directory if it does not exist

// logging and printing
typedef enum {
    VERBOSE,               // for detailed output
    ERROR,                 // for error messages
} MsgType;

void print_v(MsgType msg_type, int indent, const char* text, va_list args); // print formatted text with variable arguments
void print(MsgType msg_type, int indent, const char* text, ...);            // print formatted text with a specified message type

// error handling
void application_panic(const char* file_path, int line, const char* format, ...); // handle critical errors and terminate the application
void application_assert(const char* file, int line, Bool cond, const char* message); // assert a condition and panic if it fails
void user_panic(const char* current_filename, int line, const char* format, ...); // handle user-facing errors and terminate the application

#define USER_PANIC(current_filename, line, ...) user_panic(current_filename, line, __VA_ARGS__)
#define PANIC(...) application_panic(__FILE__, __LINE__, __VA_ARGS__)
#define UNREACHABLE() PANIC("unreachable")
#define ASSERT(cond, message) application_assert(__FILE__, __LINE__, cond, message)

// basic math macros
#define MAX(a, b) \
    a > b ? a : b

#define MIN(a, b) \
    a < b ? a : b

#endif
