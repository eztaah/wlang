#ifndef GLOBAL_H
#define GLOBAL_H

typedef unsigned char C8;
typedef unsigned int U32;
typedef int I32;
typedef int B32;
typedef float F32;
typedef char Char;
typedef void Void;

#define true 1
#define false 0

// Theses functions and macros should be used to handle internal errors
Void application_panic(const Char* file_path, I32 line, const Char* format, ...);
Void application_assert(const Char* file, I32 line, B32 cond, const Char* message);

#define PANIC(...) application_panic(__FILE__, __LINE__, __VA_ARGS__)
#define UNREACHABLE() PANIC("unreachable")
#define ASSERT(cond, message) application_assert(__FILE__, __LINE__, cond, message)

#endif