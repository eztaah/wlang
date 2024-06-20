#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "global.h"

// Global variables
bool to_executable = false;
bool dev_mode = false;
bool runtime_error_flag = false;
char *output_directory;

Token* lex(const char *code);
char* get_printable_lexer_output(Token *tokens, size_t num_tokens);


char* read_file_content(const char *file_path) 
{
    // Open the file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate memory for the entire content plus null terminator
    char *buffer = malloc(fileSize + 1);
    if (buffer == NULL) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Read the file into the buffer
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead < fileSize) {
        free(buffer);
        fclose(file);
        fprintf(stderr, "Failed to read the full file\n");
        return NULL;
    }

    buffer[fileSize] = '\0'; // Null-terminate the string
    fclose(file);
    return buffer;
}

void display_and_throw_error(const char *error_message, int line_number) 
{
    char block2[50] = ""; // Buffer for line number string

    // Handle line number
    if (line_number != -1) {
        snprintf(block2, sizeof(block2), "line %d:", line_number);
    }

    // Print formatted error message with basic color using ANSI escape codes
    fprintf(stderr, "\n\033[1m%s\033[31m error: \033[0m%s\n\n", block2, error_message);

    // Handle runtime errors by exiting or another method
    if (runtime_error_flag) {
        fprintf(stderr, "A runtime error occurred: %s\n", error_message);
        exit(1); // Exit the program with a failure status
    }
}

void print_usage(void) 
{
    printf("Usage:\n");
    printf("    compiler <file.cp> [options]    compile a source file with optional flags.\n");
    printf("    compiler --version              display compiler version and exit.\n");
    printf("    compiler --help                 display this help message and exit.\n");
    printf("\nOptions:\n");
    printf("    -o, --output-dir <directory>    specify the output directory. (default: ./out/)\n");
    printf("    -d, --dev-mode                  activate dev mode\n");
    printf("    -r, --runtime-error             throw runtime errors when an error occurs (useful for debugging)\n");
    printf("\n");
    printf("    -e, --to-executable             assemble and link the generated assembly code into an executable\n");
    printf("                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.\n");
    printf("                                        - the generated executable will only be able to run on x86_64 CPUs and in a Linux environment.\n");
    printf("\n");
}

int32_t main(int32_t argc, char *argv[])
{
    // handle base arguments
    if (argc < 2) {
        print_usage();
        return 1;
    }
    else if (strcmp(argv[1], "--help") == 0) {
        print_usage();
        return 0;
    }
    else if (strcmp(argv[1], "--version") == 0) {
        printf("compiler version 2.0\n");
        printf("\n");
        return 0;
    }

    // handle optional arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--to-executable") == 0) {
            to_executable = true;
        } 
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output-dir") == 0) {
            if (i + 1 < argc) {
                output_directory = argv[i + 1];
                i++;
            } 
            else {
                display_and_throw_error("-o or --output-dir argument used, but no directory specified", -1);
                return 1;
            }
        } 
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dev-mode") == 0) {
            dev_mode = true;
        } 
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--runtime-error") == 0) {
            runtime_error_flag = true;
        } 
        else {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "%s is not a valid argument", argv[i]);
            display_and_throw_error(error_message, -1);
            print_usage();
            return 1;
        }
    }

    // handle build directory , create directory if needed
    strcat(output_directory, "./out/");      // char *strcat(s,ct) concatenate string ct to end of string s; return s.
    struct stat st = {0};
    if (stat(output_directory, &st) == -1) {   // si le dossier n'existe pas alors : 
        if (mkdir(output_directory, 0700) != 0) {  // Crée le répertoire avec accès seulement pour le propriétaire, si erreur alors : 
            display_and_throw_error("failed to create build directory", -1);
            return 1;
        }
    }

    // open the input file
    const char *file_path = argv[1];
    char *code = read_file_content(file_path);

    if (code == NULL) {
        return 1;
    }


    // --- LEXER ---
    printf("\033[96m1. lexing... \033[0m");
    Token *tokens = lex(code);
    const char *lexer_output = get_printable_lexer_output(tokens, 3);
    // Write result to file
    char output_filename[1024];
    sprintf(output_filename, "%s1_lexer_output.txt", output_directory); // Construct file path
    FILE *output1 = fopen(output_filename, "w");
    if (output1 != NULL) {
        fprintf(output1, "%s", lexer_output);
        fclose(output1);
    } else {
        perror("Failed to open file");
    }

    printf("\033[96mdone\033[0m\n");



    free(code);
    return 0;
}