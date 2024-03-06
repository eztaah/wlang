#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "global.hpp"

std::vector<Token> lex(const std::string &code);
std::string get_printable_lexer_output(const std::vector<Token> &tokens);

#endif // LEXER_HPP
