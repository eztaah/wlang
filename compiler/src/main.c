#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "io.h"
#include "lexer.h"
#include "lib/lib.h"
#include "compiler.h"

B32 dev_mode = false;
B32 runtime_error_flag = false;
Char* output_path;

static Void print_usage(Void) {
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

static Void handle_arguments(const I32 argc, Char* argv[])
{
    // handle base arguments
    if (argc < 2) {
        print_usage();
        exit(EXIT_FAILURE);
    }
    else if (strings_are_equals(argv[1], "--help")) {
        print_usage();
        exit(EXIT_SUCCESS);
    }
    else if (strings_are_equals(argv[1], "--version")) {
        printf("compiler version 1.1\n");
        exit(EXIT_SUCCESS);
    }

    // handle optional arguments
    for (I32 i = 2; i < argc; i++) {
        if (strings_are_equals(argv[i], "-o") || strings_are_equals(argv[i], "--output")) {
            if (i + 1 < argc) {
                output_path = argv[i + 1];
                i++;
            } 
            else {
                printf("-o or --output-dir argument used, but no directory specified");
                exit(1);
            }
        } 
        else if (strings_are_equals(argv[i], "-d") || strings_are_equals(argv[i], "--dev-mode")) {
            dev_mode = true;
        } 
        else if (strings_are_equals(argv[i], "-r") || strings_are_equals(argv[i], "--runtime-error")) {
            runtime_error_flag = true;
        } 

        // if a wrong argument is given
        else {
            printf("%s is not a valid argument", argv[i]);
            print_usage();
            exit(EXIT_FAILURE);
        }
    }
}

I32 main(I32 argc, Char* argv[])
{
    // TODO: return a list with all the flags, and pass it to the compile_file function
    handle_arguments(argc, argv);

    compile_file(output_path);

    return 0;
}
