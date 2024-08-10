#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "lib.h"

Bool verbose = FALSE;
Bool dev_mode = FALSE;
Bool to_executable = FALSE;
static Char* source_path;

static Void print_usage(Void)
{
    printf("Usage:\n");
    printf("    wlangc <file.w> [options]     compile a source file with optional flags.\n");
    printf("    wlangc --version              display compiler version and exit.\n");
    printf("    wlangc --help                 display this help message and exit.\n");
    printf("\nOptions:\n");
    printf("    -v, --verbose                   output additional information\n");
    printf("    -d, --dev-mode                  activate dev mode (add comments in the asm code, ...)\n");
    printf("    -e, --to-executable             assemble and link the asmed assembly code into an executable\n");
    printf("                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.\n");
    printf("                                        - the generated executable will only run on x86_64 architecture and requires a Linux system.\n");
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
    else if (char_cmp(argv[1], "--version")) {
        printf("wlangc 0.1\n");
        exit(EXIT_SUCCESS);
    }

    // handle source file argument
    source_path = argv[1];

    // handle optional arguments
    for (I32 i = 2; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '-') {
            // handle combined arguments
            for (I32 j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'v':
                        verbose = TRUE;
                        break;
                    case 'd':
                        dev_mode = TRUE;
                        break;
                    case 'e':
                        to_executable = TRUE;
                        break;
                    default:
                        printf("-%c is not a valid argument\n", argv[i][j]);
                        print_usage();
                        exit(EXIT_FAILURE);
                }
            }
        } 
        else {
            // handle full length arguments
            if (char_cmp(argv[i], "-v") || char_cmp(argv[i], "--verbose")) {
                verbose = TRUE;
            }
            else if (char_cmp(argv[i], "-d") || char_cmp(argv[i], "--dev-mode")) {
                dev_mode = TRUE;
            }
            else if (char_cmp(argv[i], "-e") || char_cmp(argv[i], "--to-executable")) {
                to_executable = TRUE;
            }
            // if a wrong argument is given
            else {
                printf("%s is not a valid argument\n", argv[i]);
                print_usage();
                exit(EXIT_FAILURE);
            }
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
    write_file("out/lexer_out.txt", str_to_char(token_list_printable));
    str_free(token_list_printable);

    // Parsing
    List* node_list = parse(token_list);
    list_free(token_list);
    Str* node_list_printable = print_nodelist(node_list);
    write_file("out/parser_out.txt", str_to_char(node_list_printable));
    str_free(node_list_printable);

    // AsmG
    Str* asm_code = asme(node_list); // TOFIX: This function change the content of node_list and this is not normal
    list_free(node_list);
    write_file("out/asmg_out.s", str_to_char(asm_code));
    str_free(asm_code);

    // Assemble and link
    if (to_executable) {
        if (verbose) {
            printf("assembling...\n");
        }
        sh("as out/asmg_out.s -o out/as_out.o");

        if (verbose) {
            printf("linking...\n");
        }
        sh("ld -s out/as_out.o -o out/prog");
    }
}

I32 main(I32 argc, Char* argv[])
{
    handle_arguments(argc, argv); // TODO: return a list with all the flags, and pass it to the compile_file function

    compile_file(source_path);

    return 0;
}
