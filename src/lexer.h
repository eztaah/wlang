#ifndef LEXER_H
#define LEXER_H

#include "lib/lib.h"
#include "token.h"

List* lex(Char* src);
Char* convert_lexer_output_to_char(List* lexer_output);

#endif
