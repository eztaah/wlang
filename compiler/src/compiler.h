#ifndef COMPILER_H
#define COMPILER_H

#include "lib.h"
#include "node.h"
#include "token.h"

extern Bool verbose;
extern Bool dev_mode;

List* lex(Char* src);
List* parse(const List* token_list);
Str* asme(List* node_list);

#endif // COMPILER_H