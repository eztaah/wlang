#ifndef COMPILER_H
#define COMPILER_H

#include "lib.h"
#include "node.h"
#include "token.h"

extern Bool verbose;

List* lex(const Char* src);
List* parse(const List* token_list);
Str* asme(const List* fundef_node_list);

#endif // COMPILER_H