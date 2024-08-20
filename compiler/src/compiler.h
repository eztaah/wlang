#ifndef COMPILER_H
#define COMPILER_H

#include "lib.h"
#include "node.h"
#include "token.h"

extern Bool verbose;
extern Char* current_filename;

extern Bool no_libc;
extern Bool no_libw;
extern Bool static_library;

// compiler driver
Void compile_file(const Char* filename, Dict* macro_dict);
I32 assemble_file(const Char* filename, Str* object_files);
I32 link_executable(Str* object_files);

// compiler
Str* preprocess_file(const Char* filename, Dict* macro_dict);
List* lex(const Char* src);
List* parse(const List* token_list);
Void analyze_ast(List* fundef_node_list);
Str* asme(const List* fundef_node_list);

#endif