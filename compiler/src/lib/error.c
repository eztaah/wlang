#include <stdlib.h> // abort()

#include "lib.h"

Void application_panic(const Char* file_path, I32 line, const Char* format, ...)
{
    va_list args;
    va_start(args, format);

    print(MSG_ERROR, "%s:%d: APPLICATION PANIC: ", file_path, line);

    // handle end of the message
    print_v(MSG_ERROR, format, args);

    print(MSG_ERROR, "\n\n");

    va_end(args);
    abort();
}

Void application_assert(const Char* file, I32 line, Bool cond, const Char* message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}
