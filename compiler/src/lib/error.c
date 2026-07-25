#include <stdlib.h> // abort()

#include "compiler.h"
#include "lib.h"

Void application_panic(const Char* file_path, I32 line, const Char* format, ...)
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

Void application_assert(const Char* file, I32 line, Bool cond, const Char* message)
{
    if (!cond) {
        application_panic(file, line, message);
    }
}

Void user_panic(const Char* format, ...)
{
    va_list args;
    va_start(args, format);

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