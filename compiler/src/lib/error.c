#include <stdlib.h> // abort()

#include "compiler.h"
#include "lib.h"

void application_panic(const char* file_path, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    print(MSG_ERROR, "APPLICATION PANIC: %s:%d: ", file_path, line);

    // handle end of the message
    print_v(MSG_ERROR, format, args);

    print(MSG_ERROR, "\n");

    va_end(args);

#ifdef THROW_RUNTIME_ERROR
    print(MSG_ERROR, "\n");
    abort();
#else
    exit(EXIT_FAILURE);
#endif
}

void application_assert(const char* file, int line, Bool cond, const char* message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}

#define ANSI_BOLD          "\033[1m"
#define ANSI_RED_BOLD      "\033[1;31m"
#define ANSI_RESET         "\033[0m"

void user_panic(const char* current_filename, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // Print the filename and line number in bold
    print(MSG_ERROR, ANSI_BOLD "%s: line %d: " ANSI_RED_BOLD "error: " ANSI_RESET, current_filename, line);

    // Print the custom error message
    print_v(MSG_ERROR, format, args);

    // End the line with a newline character
    print(MSG_ERROR, "\n");

    va_end(args);

#ifdef THROW_RUNTIME_ERROR
    abort();
#else
    exit(EXIT_FAILURE);
#endif
}