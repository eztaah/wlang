#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "node.h"
#include "token.h"

List* parse(const List* token_list);

#endif // PARSER_H