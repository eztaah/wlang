#ifndef COMPILER_H
#define COMPILER_H

#include "lib.h"
#include "node.h"
#include "token.h"

extern B32 dev_mode;

List* lex(Char* src);
List* parse(const List* token_list);
Char* generate(const List* node_list);

#endif // COMPILER_H