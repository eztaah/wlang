#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "lib.h"

Bool dev_mode = FALSE;
Bool compile = FALSE;
static Char* source_path;

static Void print_usage(Void)
{
    printf("Usage:\n");
    printf("    compiler <file.cp> [options]    compile a source file with optional flags.\n");
    printf("    compiler --version              display compiler version and exit.\n");
    printf("    compiler --help                 display this help message and exit.\n");
    printf("\nOptions:\n");
    printf("    -d, --dev-mode                  activate dev mode\n");
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
    else if (char_cmp(argv[1], "--help")) {
        print_usage();
        exit(EXIT_SUCCESS);
    }
    else if (char_cmp(argv[1], "--version") || char_cmp(argv[1], "-v")) {
        printf("compiler version 1.1\n");
        exit(EXIT_SUCCESS);
    }

    // handle source file argument
    source_path = argv[1];

    // handle optional arguments
    for (I32 i = 2; i < argc; i++) {
        if (char_cmp(argv[i], "-d") || char_cmp(argv[i], "--dev-mode")) {
            dev_mode = TRUE;
        }
        else if (char_cmp(argv[i], "-e") || char_cmp(argv[i], "--to-executable")) {
            compile = TRUE;
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

    // Lexing
    List* token_list = lex(src);
    free(src);
    Str* token_list_printable = print_tokenlist(token_list);
    write_file("test/lexer_out.txt", str_to_char(token_list_printable));
    str_free(token_list_printable);

    // Parsing
    List* node_list = parse(token_list);
    list_free(token_list);
    Str* node_list_printable = print_nodelist(node_list);
    write_file("test/parser_out.txt", str_to_char(node_list_printable));
    str_free(node_list_printable);

    // Generator
    // Str* asm_code = generate(node_list); // TOFIX: This function change the content of node_list and this is not normal
    // list_free(node_list);
    // write_file("test/generator_out.s", str_to_char(asm_code));
    // str_free(asm_code);

    // Assemble and link
    if (compile) {
        sh("as test/generator_out.s -o test/as_out.o");
        sh("ld -s -o test/as_out.o test/prog");
    }
}

I32 main(I32 argc, Char* argv[])
{
    handle_arguments(argc, argv); // TODO: return a list with all the flags, and pass it to the compile_file function

    compile_file(source_path);

    return 0;
}
