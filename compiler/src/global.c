#include <stdio.h>
#include <stdlib.h>
#include "global.h"

// Error management related functions
Void application_panic(const char *file_path, I32 line, const char *message)
{
    printf("%s:%d: APPLICATION PANIC: %s\n", file_path, line, message);
    abort();
}

Void application_assert(const char *file, I32 line, B32 cond, const char *message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}