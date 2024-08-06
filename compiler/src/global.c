#include "global.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Void application_panic(const Char* file_path, I32 line, const Char* format, ...)
{
    va_list args;
    fprintf(stderr, "%s:%d: APPLICATION PANIC: ", file_path, line);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    abort();
}

Void application_assert(const Char* file, I32 line, B32 cond, const Char* message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}
