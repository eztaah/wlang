#include <stdio.h>  // printf()
#include <stdlib.h> // exit(), free()
#include <string.h> // strdup()

#include "compiler.h"
#include "lib.h"

#define COMPILER_VERSION "1.0.0"

// global flags for compiler options
Bool verbose = FALSE;
Bool compile_only = FALSE;
Bool no_start_fun = FALSE;
Bool no_libc = FALSE;
Bool no_libw = FALSE;
Bool static_library = FALSE;

// function to display usage information
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
    printf("    --no-start-fun                   no _start function, and do not rename main to w__main\n");
    printf("    --lib                            create a static library\n");
    printf("\n");
}

static List* handle_arguments(int argc, const char* argv[], Dict* macro_dict)
{
    List* source_files = list_new(sizeof(char*)); // initialize list to hold source files

    // if no arguments provided, or help/version flags are given, show usage/version and exit
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

    // parse each argument to set appropriate flags or collect source files
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // handle options with flags
            if (char_cmp(argv[i], "-v") || char_cmp(argv[i], "--verbose")) {
                verbose = TRUE;
            }
            else if (char_cmp(argv[i], "-d") || char_cmp(argv[i], "--define")) {
                // make sure there's a macro specified after -d or --define
                if (i + 1 >= argc) {
                    print(ERROR, 0, "No macro specified after %s\n", argv[i]);
                    exit(EXIT_FAILURE);
                }
                // store the macro definition in the dictionary
                char* macro_name = strdup(argv[++i]);
                dict_put(macro_dict, macro_name, str_to_char(str_new("")));
                free(macro_name);
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
            else if (char_cmp(argv[i], "--no-start-fun")) {
                no_start_fun = TRUE;
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
            // assume non-flag arguments are source files
            list_push(source_files, &argv[i]);
        }
    }

    // if no source files were provided, show an error and exit
    if (source_files->size == 0) {
        printf("No source files specified\n");
        print_usage();
        exit(EXIT_FAILURE);
    }

    // if verbose mode is enabled, print a summary of the options used
    print(VERBOSE, 0, "command: \n");
    print(VERBOSE, -1, "    ");
    for (int i = 0; i < argc; i++) {
        print(VERBOSE, -1, "%s ", argv[i]);
    }
    print(VERBOSE, 0, "\n");

    print(VERBOSE, 0, "options summary:\n");
    print(VERBOSE, 1, "--verbose:      %s\n", verbose ? "used" : "not used");
    print(VERBOSE, 1, "--compile-only: %s\n", compile_only ? "used" : "not used");
    print(VERBOSE, 1, "--no-start-fun: %s\n", no_start_fun ? "used" : "not used");
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

    return source_files; // return the list of source files to be compiled
}

int main(int argc, const char* argv[])
{
    Dict* macro_dict = dict_new();

    List* source_files = handle_arguments(argc, argv, macro_dict);

    // compile each source file provided
    for (int i = 0; i < source_files->size; i++) {
        char* filename = *(char**)list_get(source_files, i);
        print(VERBOSE, 0, "compiling %s\n", filename);
        compile_file(filename, macro_dict);
    }

    // check if we need to proceed with assembling and linking
    if (!compile_only) {
        Str* object_files = str_new("");

        for (int i = 0; i < source_files->size; i++) {
            char* filename = *(char**)list_get(source_files, i);

            // replace the extension .w with .s for assembly, if applicable
            char* dot = strrchr(filename, '.');
            if (dot && strcmp(dot, ".w") == 0) {
                *dot = '\0'; // temporarily remove the .w extension
                print(VERBOSE, 0, "assembling %s.s\n", filename);
                *dot = '.'; // restore the original filename
            }
            else {
                print(VERBOSE, 0, "assembling %s\n", filename); // log assembly process for files without .w extension
            }

            int ret_code = assemble_file(filename, object_files); // assemble the file
            if (ret_code != 0) {
                // list_free(source_files);
                dict_free(macro_dict);
                str_free(object_files);
                return ret_code;
            }
        }

        print(VERBOSE, 0, "linking\n");
        int ret_code = link_executable(object_files, argv[0]); // link object files to create the final executable
        if (ret_code != 0) {
            // list_free(source_files);
            dict_free(macro_dict);
            str_free(object_files);
            return ret_code;
        }

        str_free(object_files);
    }

    print(VERBOSE, -1, "\n");

    // provide final feedback to the user about the output location
    printf("compilation completed. the generated ");
    if (compile_only) {
        printf("assembly code is located in ./out/\n");
    }
    else if (static_library) {
        printf("shared library is located in ./out/lib.a.\n");
    }
    else {
        printf("executable is located in ./out/prog.\n");
    }

    // list_free(source_files);
    dict_free(macro_dict);

    return 0;
}
