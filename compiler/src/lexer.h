#ifndef LEXER_H
#define LEXER_H

#include "lib/lib.h"
#include "token.h"

/**
 * On lui donne le code source et il renvoie une liste de tokens
 */
List* lex(Char* src);

Char* convert_token_list_to_char(List* lexer_output);

#endif // LEXER_H
