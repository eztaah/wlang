#include <stdio.h>
#include <stdlib.h>

#include "generator.h"
#include "global.h"
#include "io.h"
#include "lexer.h"
#include "lib/lib.h"
#include "parser.h"
#include "utils.h"

static B32 dev_mode = false;
static Char* output_path;
static Char* source_path;

static Void print_usage(Void)
{
    printf("Usage:\n");
    printf("    compiler <file.cp> [options]    compile a source file with optional flags.\n");
    printf("    compiler --version              display compiler version and exit.\n");
    printf("    compiler --help                 display this help message and exit.\n");
    printf("\nOptions:\n");
    printf("    -o, --output-dir <directory>    specify the output directory. (default: ./out/)\n");
    printf("    -d, --dev-mode                  activate dev mode\n");
    printf("\n");
    printf("    -e, --to-executable             assemble and link the generated assembly code into an executable\n");
    printf("                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.\n");
    printf("                                        - the generated executable will only run on x86_64 architecture and requires a Linux system with the GNU C Library (glibc).\n");
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
    else if (strings_are_equals(argv[1], "--version") || strings_are_equals(argv[1], "-v")) {
        printf("compiler version 1.1\n");
        exit(EXIT_SUCCESS);
    }

    // handle source file argument
    source_path = argv[1];

    // handle optional arguments
    for (I32 i = 2; i < argc; i++) {
        if (strings_are_equals(argv[i], "-o") || strings_are_equals(argv[i], "--output")) {
            if (i + 1 < argc) {
                output_path = argv[i + 1];
                i++;
            }
            else {
                printf("-o or --output-dir argument used, but no directory specified");
                exit(EXIT_FAILURE);
            }
        }
        else if (strings_are_equals(argv[i], "-d") || strings_are_equals(argv[i], "--dev-mode")) {
            dev_mode = true;
        }
        // if a wrong argument is given
        else {
            printf("%s is not a valid argument", argv[i]);
            print_usage();
            exit(EXIT_FAILURE);
        }
    }
}

static Void compile_file(const Char* filename)
{
    Char* src = read_file(filename);

    List* token_list = lex(src);
    write_file("out/lexer_output.txt", print_tokenlist(token_list));

    List* node_list = parse(token_list);
    write_file("out/parser_output.txt", print_nodelist(node_list));

    Char* asm_code = generate(node_list);   // TOFIX: This function change the content of node_list and this is not normal
    write_file("out/generator_output.s", asm_code);

    // Clean up
    destroy_list(token_list);
    destroy_list(node_list);
    free(asm_code);
    free(src);
}

I32 main(I32 argc, Char* argv[])
{
    handle_arguments(argc, argv); // TODO: return a list with all the flags, and pass it to the compile_file function

    compile_file(source_path);

    return 0;
}
