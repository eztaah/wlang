#include <stdlib.h> // abort()

#include "compiler.h"
#include "lib.h"

void application_panic(const char* file_path, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // print panic location
    print(ERROR, 0, "APPLICATION PANIC: %s:%d: ", file_path, line);

    // print panic message
    print_v(ERROR, 0, format, args);

    print(ERROR, 0, "\n");

    va_end(args);

#ifdef THROW_RUNTIME_ERROR
    print(ERROR, 0, "\n");
    abort(); // terminates the program with core dump
#else
    exit(EXIT_FAILURE); // terminates the program without core dump
#endif
}

void application_assert(const char* file, int line, Bool cond, const char* message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}

#define ANSI_BOLD "\033[1m"
#define ANSI_RED_BOLD "\033[1;31m"
#define ANSI_RESET "\033[0m"

void user_panic(const char* current_filename, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // print error with formatting
    print(ERROR, 0, ANSI_BOLD "%s: line %d: " ANSI_RED_BOLD "error: " ANSI_RESET, current_filename, line);

    // print custom error message
    print_v(ERROR, 0, format, args);

    // terminate the message
    print(ERROR, 0, "\n");

    va_end(args);

#ifdef THROW_RUNTIME_ERROR
    abort(); // terminates the program with core dump
#else
    exit(EXIT_FAILURE); // terminates the program without core dump
#endif
}
