// lexer.hpp
#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

typedef enum {
    UNDEFINED,
    QUOTE,
    LET,
    VAR,
    NUMBER,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    EQUALS,
    EQUALS_EQUALS,
    SEMICOLON,
    COMMA,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    IF,
    IDENTIFIER,
    EOF_TOKEN
} TokenType;

typedef std::pair<TokenType, std::string> Token;

std::vector<Token> lexer(const std::string &code);
std::string tokenTypeToString(TokenType type);
std::string get_lexer_output(const std::vector<Token> &tokens);

#endif // LEXER_HPP
