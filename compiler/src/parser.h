#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "token.h"
#include "node.h"

/**
 * On lui donne la liste des tokens et il renvoie une liste de statement nodes
 */
List* parse(const List* token_list);

#endif // PARSER_H