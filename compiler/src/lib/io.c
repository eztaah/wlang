#define _GNU_SOURCE

#include <stdio.h>    // printf()
#include <stdlib.h>   // malloc(), free()
#include <string.h>   // strcat()
#include <sys/stat.h> // stat

#include "compiler.h" // dev_mode, verbose
#include "lib.h"

Char* read_file(const Char* filename)
{
    FILE* fp;
    Char* line = NULL;
    UX len = 0;
    IX read;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        print(MSG_ERROR, "Could not read file `%s`\n", filename);
        exit(EXIT_FAILURE);
    }

    Char* buffer = (Char*)calloc(1, sizeof(Char));
    buffer[0] = '\0';

    while ((read = getline(&line, &len, fp)) != -1) {
        buffer = (Char*)realloc(buffer, (strlen(buffer) + strlen(line) + 1) * sizeof(Char));
        strcat(buffer, line);
    }

    fclose(fp);
    if (line) {
        free(line);
    }

    return buffer;
}

Void write_file(const Char* filename, Char* outbuffer)
{
    FILE* fp;

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        print(MSG_ERROR, "Could not open file for writing `%s`\n", filename);
        exit(EXIT_FAILURE);
    }

    fputs(outbuffer, fp);

    fclose(fp);
}

Char* sh(const Char* cmd)
{
    Char* output = (Char*)calloc(1, sizeof(Char));
    output[0] = '\0';

    FILE* fp;
    Char path[1035];

    fp = popen(cmd, "r");

    if (fp == NULL) {
        print(MSG_ERROR, "Failed to run command\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        output = (Char*)realloc(output, (strlen(output) + strlen(path) + 1) * sizeof(Char));
        strcat(output, path);
    }

    pclose(fp);

    return output;
}

Void create_dir(const Char* dir)
{
    struct stat st = {0};

    // check if folder already exist
    if (stat(dir, &st) == -1) {
        // create folder with 0755 permissions (read, write et execute for the owner, read and execute for other)
        if (mkdir(dir, 0755) != 0) {
            PANIC("failed to create output directory");
        }
    }
}

#define MAX_LOG_LENGTH 1024
void print_v(MsgType msg_type, const char* text, va_list args)
{
    char buffer[MAX_LOG_LENGTH];

    // using vsnprintf to format the text
    vsnprintf(buffer, MAX_LOG_LENGTH, text, args);

    switch (msg_type) {
        case MSG_STEP:
            if (verbose) {
                fprintf(stdout, "%s", buffer);
            }
            break;

        case MSG_INFO:
            if (verbose) {
                fprintf(stdout, "   %s", buffer);
            }
            break;

        case MSG_ERROR:
            if (verbose) {
                fprintf(stderr, "   %s", buffer);
            }
            else {
                fprintf(stderr, "%s", buffer);
            }
            break;

        default:
            PANIC("unknown message type");
            break;
    }
}

void print(MsgType msg_type, const char* text, ...)
{
    va_list args;
    va_start(args, text);
    print_v(msg_type, text, args);
    va_end(args);
}