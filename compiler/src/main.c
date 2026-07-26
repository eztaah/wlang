#include <stdio.h>  // printf()
#include <stdlib.h> // exit(), free()
#include <string.h>

#include "compiler.h"
#include "lib.h"

Bool verbose = FALSE;
Bool to_executable = FALSE;
static List* source_files;

static Void print_usage(Void)
{
    printf("Usage:\n");
    printf("    wlangc <file.w> [options]     compile a source file with optional flags.\n");
    printf("    wlangc --version              display compiler version and exit.\n");
    printf("    wlangc --help                 display this help message and exit.\n");
    printf("\nOptions:\n");
    printf("    -v, --verbose                   output additional information\n");
    printf("    -e, --to-executable             assemble and link the asmed assembly code into an executable\n");
    printf("                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.\n");
    printf("                                        - the generated executable will only run on x86_64 architecture and requires a Linux system.\n");
    printf("\n");
}

static Void handle_arguments(I32 argc, const Char* argv[], DictStr* macro_dict)
{
    source_files = list_new(sizeof(Char*));

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
        printf("wlangc 0.3\n");
        exit(EXIT_SUCCESS);
    }

    // handle arguments
    for (I32 i = 1; i < argc; i++) {
        // handle options arguments
        if (argv[i][0] == '-') {
            // handle full length arguments or combined flags
            if (char_cmp(argv[i], "-v") || char_cmp(argv[i], "--verbose")) {
                verbose = TRUE;
            }
            else if (char_cmp(argv[i], "-e") || char_cmp(argv[i], "--to-executable")) {
                to_executable = TRUE;
            }
            else if (argv[i][1] == 'D') {
                // Define a macro
                Char* macro_name = strdup(argv[i] + 2); // Get the macro name after '-D'
                dictstr_put(macro_dict, macro_name, str_to_char(str_new("")));
            }
            else {
                // handle combined flags like -vd
                for (I32 j = 1; argv[i][j] != '\0'; j++) {
                    switch (argv[i][j]) {
                        case 'v':
                            verbose = TRUE;
                            break;
                        case 'e':
                            to_executable = TRUE;
                            break;
                        default:
                            print(MSG_ERROR, "-%c is not a valid argument\n", argv[i][j]);
                            print_usage();
                            exit(EXIT_FAILURE);
                    }
                }
            }
        }
        // hangle source file argument
        else {
            // assume it is a source file
            list_push(source_files, &argv[i]);
        }
    }

    if (source_files->size == 0) {
        print(MSG_ERROR, "No source files specified\n");
        print_usage();
        exit(EXIT_FAILURE);
    }
}

static Void compile_file(const Char* filename, DictStr* macro_dict)
{
    create_dir("out");

    dictstr_print(macro_dict);

    // Extract just the filename without the path
    Char* out_filename = strdup(filename);
    Char* slash = strrchr(out_filename, '/');
    if (slash) {
        out_filename = strdup(slash + 1); // Duplicate the string starting after the last '/'
    }

    // Remove the extension from the base filename
    Char* dot = strrchr(out_filename, '.');
    if (dot)
        *dot = '\0'; // Remove the extension

    // Create the final output path starting with "./out/"
    Str* final_output_path = str_new("out/");
    str_cat(final_output_path, out_filename);

    // Preprocess the file
    Str* preprocessed_content = preprocess_file(filename, macro_dict);

    // Write the preprocessed content to a file
    Str* preprocessed_out_file = str_new(str_to_char(final_output_path));
    str_cat(preprocessed_out_file, ".pp"); // .pp as a typical extension for preprocessed files
    write_file(str_to_char(preprocessed_out_file), str_to_char(preprocessed_content));
    str_free(preprocessed_out_file);

    // Now proceed with the lexing using the preprocessed content
    Char* src = str_to_char(preprocessed_content);

    // Lexing
    List* token_list = lex(src);
    str_free(preprocessed_content); // Free preprocessed content
    Str* token_list_printable = print_tokenlist(token_list);

    // Print lexer output to a file
    Str* lexer_out_file = str_new(str_to_char(final_output_path));
    str_cat(lexer_out_file, ".lex");
    write_file(str_to_char(lexer_out_file), str_to_char(token_list_printable));
    str_free(token_list_printable);
    str_free(lexer_out_file);

    // Parsing
    List* node_list = parse(token_list);
    list_free(token_list);
    Str* node_list_printable = print_nodelist(node_list);

    // Print parser output to a file
    Str* parser_out_file = str_new(str_to_char(final_output_path));
    str_cat(parser_out_file, ".ast");
    write_file(str_to_char(parser_out_file), str_to_char(node_list_printable));
    str_free(node_list_printable);
    str_free(parser_out_file);

    // AsmG
    Str* asm_code = asme(node_list); // TOFIX: This function changes the content of node_list and this is not normal
    list_free(node_list);

    // Print assembler output to a file
    Str* asm_out_file = str_new(str_to_char(final_output_path));
    str_cat(asm_out_file, ".s");
    write_file(str_to_char(asm_out_file), str_to_char(asm_code));
    str_free(asm_code);
    str_free(asm_out_file);

    free(out_filename);
}

I32 main(I32 argc, const Char* argv[])
{
    DictStr* macro_dict = dictstr_new();

    handle_arguments(argc, argv, macro_dict);

    // Compile each file
    for (I32 i = 0; i < source_files->size; i++) {
        Char* filename = *(Char**)list_get(source_files, i);
        compile_file(filename, macro_dict);
    }

    // assemble and link
    if (to_executable) {
        Str* object_files = str_new("");

        for (I32 i = 0; i < source_files->size; i++) {
            Char* filename = *(Char**)list_get(source_files, i);

            // extract just the filename without the path
            Char* out_filename = strdup(filename);
            Char* slash = strrchr(out_filename, '/');
            if (slash) {
                out_filename = strdup(slash + 1); // duplicate the string starting after the last '/'
            }
            // remove the extension from the base filename
            Char* dot = strrchr(out_filename, '.');
            if (dot)
                *dot = '\0'; // Remove the extension

            // assemble each file
            Str* asm_out_file = str_new("out/");
            str_cat(asm_out_file, out_filename);
            str_cat(asm_out_file, ".s");

            Str* object_file = str_new("out/");
            str_cat(object_file, out_filename);
            str_cat(object_file, ".o");

            str_cat(object_files, str_to_char(object_file));
            str_cat(object_files, " ");

            print(MSG_STEP, "assembling %s...\n", str_to_char(asm_out_file));
            Str* assemble_cmd = str_new("as ");
            str_cat(assemble_cmd, str_to_char(asm_out_file));
            str_cat(assemble_cmd, " -o ");
            str_cat(assemble_cmd, str_to_char(object_file));
            sh(str_to_char(assemble_cmd));

            str_free(assemble_cmd);
            str_free(asm_out_file);
            str_free(object_file);
            free(out_filename);
        }

        // linking all object files into a single executable
        Str* link_cmd = str_new("ld -o out/prog ");
        str_cat(link_cmd, str_to_char(object_files));
        str_cat(link_cmd, "-lc -dynamic-linker /lib64/ld-linux-x86-64.so.2");

        print(MSG_STEP, "linking...\n");
        sh(str_to_char(link_cmd));


        dictstr_free(macro_dict);
        str_free(link_cmd);
        str_free(object_files);
    }

    // clean up
    // list_free(source_files);
    return 0;
}
