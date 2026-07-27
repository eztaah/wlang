#ifndef COMPILER_H
#define COMPILER_H

#include "lib.h"
#include "node.h"
#include "token.h"

extern Bool verbose;

List* lex(const Char* src);
List* parse(const List* token_list);
Str* asme(const List* fundef_node_list);
Str* preprocess_file(const Char* filename, Dict* macro_dict);
Void write_preprocessed_file(const Char* base_dir, const Char* filename, const Str* processed_content);
Void analyze_ast(List* fundef_node_list);

#endif // COMPILER_H