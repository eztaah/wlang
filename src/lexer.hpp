// lexer.hpp
#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>


typedef std::pair<std::string, std::string> Token;

std::vector<Token> lexer(const std::string& code);
void pretty_print(const std::vector<Token>& tokens);

#endif // LEXER_HPP
