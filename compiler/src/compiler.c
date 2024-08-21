#define _GNU_SOURCE
#include <unistd.h>

#include <stdio.h>  // printf()
#include <stdlib.h> // exit(), free()
#include <string.h>

#include "lib.h"
#include "compiler.h"

char* current_filename = NULL;

static Str* create_output_filename(const char* filename)
{
    // Extract just the filename without the path
    char* out_filename = strdup(filename);
    char* slash = strrchr(out_filename, '/');
    if (slash) {
        // Keep only the portion after the last slash
        char* temp = strdup(slash + 1);
        free(out_filename);
        out_filename = temp;
    }

    // Remove the extension from the base filename
    char* dot = strrchr(out_filename, '.');
    if (dot) {
        *dot = '\0';
    }

    // Create the final output path
    Str* final_output_path = str_new("out");
    str_cat(final_output_path, "/");
    str_cat(final_output_path, out_filename);

    free(out_filename);
    return final_output_path;
}

static void write_to_file(const Str* base_path, const char* extension, const char* content)
{
    Str* out_file = str_new(str_to_char(base_path));
    str_cat(out_file, extension);
    write_file(str_to_char(out_file), content);
    str_free(out_file);
}

void compile_file(const char* filename, Dict* macro_dict)
{
    current_filename = strdup(filename);

    create_dir("out");

    // Create the final output path
    Str* output_filename = create_output_filename(filename);

    // Preprocess the file
    print(VERBOSE, 1, "preprocessing\n");
    Str* preprocessed_content = preprocess_file(filename, macro_dict);
    write_to_file(output_filename, "_pp.w", str_to_char(preprocessed_content));

    // Lexing
    print(VERBOSE, 1, "lexing\n");
    char* src = str_to_char(preprocessed_content);
    List* token_list = lex(src);
    str_free(preprocessed_content); // Free preprocessed content
    Str* token_list_printable = print_tokenlist(token_list);
    write_to_file(output_filename, "_tokens.txt", str_to_char(token_list_printable));
    str_free(token_list_printable);

    // Parsing
    print(VERBOSE, 1, "parsing\n");
    List* node_list = parse(token_list);
    list_free(token_list);
    Str* node_list_printable = print_nodelist(node_list);

    // semantic analysis
    print(VERBOSE, 1, "performing semantic analysis\n");
    analyze_ast(node_list);
    write_to_file(output_filename, "_ast.txt", str_to_char(node_list_printable));
    str_free(node_list_printable);

    // AsmG
    print(VERBOSE, 1, "generating assembly\n");
    Str* asm_code = asme(node_list);
    list_free(node_list);
    write_to_file(output_filename, ".s", str_to_char(asm_code));
    str_free(asm_code);

    print(VERBOSE, 1, "file compiled successfully, assembly code in %s.s\n", str_to_char(output_filename));

    str_free(output_filename);
}

int assemble_file(const char* filename, Str* object_files)
{
    // Create paths for the .s (assembly) and .o (object) files
    Str* asm_out_file = create_output_filename(filename);
    str_cat(asm_out_file, ".s");

    Str* object_file = create_output_filename(filename);
    str_cat(object_file, ".o");

    // Append the object file path to the object_files list
    str_cat(object_files, str_to_char(object_file));
    str_cat(object_files, " ");

    // Build the assembly command
    Str* assemble_cmd = str_new("as ");
    str_cat(assemble_cmd, str_to_char(asm_out_file));

    str_cat(assemble_cmd, " -o ");
    str_cat(assemble_cmd, str_to_char(object_file));
    print(VERBOSE, 1, "$ %s\n", str_to_char(assemble_cmd));

    // Execute the assembly command
    CommandResult res = sh(str_to_char(assemble_cmd));

    // Free memory
    str_free(assemble_cmd);
    str_free(asm_out_file);
    str_free(object_file);

    return res.return_code;
}

char* get_executable_path() {
    // Buffer pour stocker le chemin complet de l'exécutable
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    
    if (len != -1) {
        buffer[len] = '\0';
        return strdup(buffer);
    } else {
        // Gestion d'erreur si la lecture du chemin échoue
        perror("readlink");
        exit(EXIT_FAILURE);
    }
}

char* get_executable_dir() {
    char* exec_path = get_executable_path();
    char* last_slash = strrchr(exec_path, '/');
    
    if (last_slash != NULL) {
        *last_slash = '\0';  // Terminer la chaîne juste avant le dernier '/'
        return exec_path;
    } 
    else {
        free(exec_path);
        PANIC("failed to determine executable location");
        return NULL;
    }
}


int link_executable(Str* object_files)
{

    Str* link_cmd;

    if (static_library) {
        // Create the linking command for a static library
        link_cmd = str_new("ar rcs out/lib.a ");
        str_cat(link_cmd, str_to_char(object_files));
    } 
    else {
        // Create the linking command for a regular executable
        link_cmd = str_new("ld -o out/prog ");
        str_cat(link_cmd, str_to_char(object_files));

        // Add libc if not disabled
        if (!no_libw) {
            char* exec_dir = get_executable_dir();
            str_cat(link_cmd, "-L");
            str_cat(link_cmd, exec_dir);
            str_cat(link_cmd, " -l:libw.a ");

            // str_cat(link_cmd, "-L/home/antoine/Documents -l:libw.a ");
        }
        if (!no_libc) {
            str_cat(link_cmd, "-lc -dynamic-linker /lib64/ld-linux-x86-64.so.2");
        }
    }


    // Print linking message and command
    print(VERBOSE, 1, "$ %s\n", str_to_char(link_cmd));

    // Execute the linking command
    CommandResult res = sh(str_to_char(link_cmd));

    str_free(link_cmd);

    return res.return_code;
}

