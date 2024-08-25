#define _GNU_SOURCE

#include <stdio.h>    // printf()
#include <stdlib.h>   // malloc(), free()
#include <string.h>   // strcat()
#include <sys/stat.h> // stat

#include "compiler.h" // verbose
#include "lib.h"

void* safe_malloc(size_t size)
{
    void* ptr = malloc(size);
    if (!ptr) {
        PANIC("Error: failed to allocate %zu bytes of memory\n", size);
    }
    return ptr;
}

void* safe_calloc(size_t num, size_t size)
{
    void* ptr = calloc(num, size);
    if (!ptr) {
        PANIC("Error: failed to allocate memory for %zu elements of size %zu bytes\n", num, size);
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t new_size)
{
    void* new_ptr = realloc(ptr, new_size);
    if (!new_ptr && new_size > 0) {
        PANIC("Error: failed to reallocate memory to %zu bytes\n", new_size);
    }
    return new_ptr;
}

char* read_file(const char* filename)
{
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        print(ERROR, 0, "Could not read file `%s`\n", filename);
        exit(EXIT_FAILURE);
    }

    char* buffer = (char*)calloc(1, sizeof(char));
    buffer[0] = '\0';

    while ((read = getline(&line, &len, fp)) != -1) {
        buffer = (char*)safe_realloc(buffer, (strlen(buffer) + strlen(line) + 1) * sizeof(char));
        strcat(buffer, line);
    }

    fclose(fp);
    if (line) {
        free(line);
    }

    return buffer;
}

void write_file(const char* filename, const char* outbuffer)
{
    FILE* fp;

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        print(ERROR, 0, "Could not open file for writing `%s`\n", filename);
        exit(EXIT_FAILURE);
    }

    fputs(outbuffer, fp);

    fclose(fp);
}

CommandResult sh(const char* cmd)
{
    CommandResult result;
    result.output = (char*)calloc(1, sizeof(char));
    result.output[0] = '\0';
    result.return_code = -1;

    FILE* fp;
    char path[1035];

    fp = popen(cmd, "r");

    if (fp == NULL) {
        PANIC("failed to run the command");
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        result.output = (char*)safe_realloc(result.output, (strlen(result.output) + strlen(path) + 1) * sizeof(char));
        strcat(result.output, path);
    }

    // capture the return code of the command
    int status = pclose(fp);
    if (status == -1) {
        PANIC("failed to close the command stream");
    }

    if (WIFEXITED(status)) {
        result.return_code = WEXITSTATUS(status);
    }
    else {
        PANIC("Command did not exit properly");
    }

    return result;
}

void create_dir(const char* dir)
{
    struct stat st = {0};

    // check if folder already exist
    if (stat(dir, &st) == -1) {
        // create folder with 0755 permissions (read, write, execute for the owner, read, and execute for others)
        if (mkdir(dir, 0755) != 0) {
            PANIC("failed to create output directory");
        }
    }
}

#define MAX_LOG_LENGTH 1024

void print_v(MsgType msg_type, int indent, const char* text, va_list args)
{
    char buffer[MAX_LOG_LENGTH];
    char indented_buffer[MAX_LOG_LENGTH + 40]; // Extra space for indentation, assuming max indent level is small

    // Using vsnprintf to format the text
    vsnprintf(buffer, MAX_LOG_LENGTH, text, args);

    // create indentation prefix (4 spaces per indent level)
    unsigned int indent_spaces = 0;
    if (indent > 0) {
        indent_spaces = indent * 4;
    }
    if (indent_spaces > sizeof(indented_buffer) - MAX_LOG_LENGTH - 1) {
        indent_spaces = sizeof(indented_buffer) - MAX_LOG_LENGTH - 1; // ensure it fits
    }
    memset(indented_buffer, ' ', indent_spaces);
    indented_buffer[indent_spaces] = '\0'; // null-terminate the string

    // concatenate the indentation and the formatted message
    strncat(indented_buffer, buffer, MAX_LOG_LENGTH - indent_spaces);

    switch (msg_type) {
        case VERBOSE:
            if (verbose) {
                if (indent == 0) {
                    // print in bold if indent is 0
                    fprintf(stdout, "\033[1m%s\033[0m", indented_buffer); // \033[1m starts bold, \033[0m ends it
                }
                else {
                    fprintf(stdout, "%s", indented_buffer);
                }
            }
            break;

        case ERROR:
            fprintf(stderr, "%s", buffer); // no indentation for errors
            break;

        default:
            PANIC("unknown message type");
            break;
    }
}

void print(MsgType msg_type, int indent, const char* text, ...)
{
    va_list args;
    va_start(args, text);
    print_v(msg_type, indent, text, args);
    va_end(args);
}
