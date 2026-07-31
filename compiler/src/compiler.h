#ifndef COMPILER_H
#define COMPILER_H

#include "lib.h"
#include "node.h"
#include "token.h"

extern Bool verbose;
extern char* current_filename;

extern Bool no_libc;
extern Bool no_libw;
extern Bool no_start_fun;
extern Bool static_library;

// compiler driver
void compile_file(const char* filename, Dict* macro_dict);
int assemble_file(const char* filename, Str* object_files);
int link_executable(Str* object_files);

// compiler
Str* preprocess_file(const char* filename, Dict* macro_dict);
List* lex(const char* src);
List* parse(const List* token_list);
void analyze_ast(List* fundef_node_list);
Str* asme(const List* fundef_node_list);

#endif