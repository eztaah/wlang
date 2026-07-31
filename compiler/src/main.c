#include <stdio.h>  // printf()
#include <stdlib.h> // exit(), free()
#include <string.h>

#include "compiler.h"
#include "lib.h"

#define COMPILER_VERSION "0.3.0"

Bool verbose = FALSE;
Bool compile_only = FALSE;
Bool no_libc = FALSE;
Bool no_libw = FALSE;
Bool static_library = FALSE;

static void print_usage(void)
{
    printf("Usage:\n");
    printf("    wlangc <file.w> [options]     compile a source file with optional flags.\n");
    printf("    wlangc --version              display compiler version and exit.\n");
    printf("    wlangc --help                 display this help message and exit.\n");
    printf("\nOptions:\n");
    printf("    -v, --verbose                    output information during compilation\n");
    printf("    -d, --define <macro>             define a macro\n");
    printf("    --no-libc                        do not link with the libc\n");
    printf("    --compile-only                   compile only; do not assemble or link\n");
    printf("    --lib                            create a static library\n");
    printf("\n");
}

static List* handle_arguments(int argc, const char* argv[], Dict* macro_dict)
{
    List* source_files = list_new(sizeof(char*));

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
        printf("wlangc %s\n", COMPILER_VERSION);
        exit(EXIT_SUCCESS);
    }

    // handle arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (char_cmp(argv[i], "-v") || char_cmp(argv[i], "--verbose")) {
                verbose = TRUE;
            }
            else if (char_cmp(argv[i], "-d") || char_cmp(argv[i], "--define")) {
                if (i + 1 >= argc) {
                    print(ERROR, 0, "No macro specified after %s\n", argv[i]);
                    exit(EXIT_FAILURE);
                }
                char* macro_name = strdup(argv[++i]);
                dict_put(macro_dict, macro_name, str_to_char(str_new("")));
            }
            else if (char_cmp(argv[i], "--no-libc")) {
                no_libc = TRUE;
            }
            else if (char_cmp(argv[i], "--no-libw")) {
                no_libw = TRUE;
            }
            else if (char_cmp(argv[i], "--compile-only")) {
                compile_only = TRUE;
            }
            else if (char_cmp(argv[i], "--lib")) {
                static_library = TRUE;
            }
            else {
                printf("%s is not a valid option\n", argv[i]);
                print_usage();
                exit(EXIT_FAILURE);
            }
        }
        else {
            // assume it is a source file
            list_push(source_files, &argv[i]);
        }
    }

    if (source_files->size == 0) {
        printf("No source files specified\n");
        print_usage();
        exit(EXIT_FAILURE);
    }

    // Verbose: print the full command line used
    print(VERBOSE, 0, "command: \n");
    print(VERBOSE, -1, "    ");
    for (int i = 0; i < argc; i++) {
        print(VERBOSE, -1, "%s ", argv[i]);
    }
    print(VERBOSE, 0, "\n");

    // Verbose summary of the options used
    print(VERBOSE, 0, "options summary:\n");
    print(VERBOSE, 1, "--verbose:      %s\n", verbose ? "used" : "not used");
    print(VERBOSE, 1, "--compile-only: %s\n", compile_only ? "used" : "not used");
    print(VERBOSE, 1, "--no-libc:      %s\n", no_libc ? "used" : "not used");
    print(VERBOSE, 1, "--no-libw:      %s\n", no_libw ? "used" : "not used");
    print(VERBOSE, 1, "--lib:          %s\n", static_library ? "used" : "not used");

    print(VERBOSE, 1, "macros defined : ");
    if (macro_dict->size == 0) {
        print(VERBOSE, -1, "/\n");
    }
    else {
        print(VERBOSE, -1, "\n");
        for (int i = 0; i < macro_dict->size; i++) {
            DictEntry* entry = macro_dict->entries[i];
            print(VERBOSE, 2, "%s\n", entry->key);
        }
    }

    print(VERBOSE, 1, "source files:\n");
    for (int i = 0; i < source_files->size; i++) {
        char* filename = *(char**)list_get(source_files, i);
        print(VERBOSE, 2, "%s\n", filename);
    }

    return source_files;
}

int main(int argc, const char* argv[])
{
    Dict* macro_dict = dict_new();

    List* source_files = handle_arguments(argc, argv, macro_dict);

    // Compile each file
    for (int i = 0; i < source_files->size; i++) {
        char* filename = *(char**)list_get(source_files, i);
        print(VERBOSE, 0, "compiling %s\n", filename);
        compile_file(filename, macro_dict);
    }

    // Check if we need to assemble and link
    if (!compile_only) {
        Str* object_files = str_new("");

        for (int i = 0; i < source_files->size; i++) {
            char* filename = *(char**)list_get(source_files, i);

            // Replace the extension .w with .s for logging
            char* dot = strrchr(filename, '.');
            if (dot && strcmp(dot, ".w") == 0) {
                *dot = '\0';  // Remove the .w
                print(VERBOSE, 0, "assembling %s.s\n", filename);
                *dot = '.';  // restore the filename with .w 
            } else {
                print(VERBOSE, 0, "assembling %s\n", filename);  // fallback if no .w extension found
            }

            int ret_code = assemble_file(filename, object_files);
            if (ret_code != 0) {
                return ret_code;
            }
        }

        print(VERBOSE, 0, "linking\n");
        int ret_code = link_executable(object_files);
        if (ret_code != 0) {
            return ret_code;
        }

        str_free(object_files);
    }

    print(VERBOSE, -1, "\n");

    if (static_library) {
        printf("compilation completed. the generated shared library is located in ./out/lib.a.\n");
    } 
    else {
        printf("compilation completed. the generated executable is located in ./out/prog.\n");
    }

    // clean up
    // list_free(source_files);
    // list_free(assembler_options);
    // list_free(linker_options);
    return 0;
}

