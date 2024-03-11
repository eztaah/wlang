#ifndef LEXER_HH
#define LEXER_HH

#include <string>
#include <vector>

#include "global.hh"


std::vector<Token> lex(const std::string &code);
std::string get_printable_lexer_output(const std::vector<Token> &tokens);

#endif // LEXER_HH
