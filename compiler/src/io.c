#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "global.h"

Char* read_file(const Char* filename)
{
    FILE* fp;
    Char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Could not read file `%s`\n", filename);
        exit(1);
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
        printf("Could not open file for writing `%s`\n", filename);
        exit(1);
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
        printf("Failed to run command\n");
        exit(1);
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        output = (Char*)realloc(output, (strlen(output) + strlen(path) + 1) * sizeof(Char));
        strcat(output, path);
    }

    pclose(fp);

    return output;
}
