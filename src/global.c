#include <stdio.h>
#include <stdlib.h>
#include "global.h"

// Error management related functions
void application_panic(const char *file_path, I32 line, const char *message)
{
    printf("%s:%d: APPLICATION PANIC: %s\n", file_path, line, message);
    abort();
}

void application_assert(const char *file, I32 line, B32 cond, const char *message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}