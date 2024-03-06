#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <string>

typedef enum {
    CST,
    VAR,
    NUMBER,
    IDENTIFIER,
    EQUALS,

    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    BIN_AND,
    BIN_OR,
    XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,

    WHILE,
    IF,
    ELSE,

    SEMICOLON,
    COMMA,
    QUOTE,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    EQUALS_EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    LESS_THAN_EQUALS,
    GREATER_THAN,
    GREATER_THAN_EQUALS,

    UNDEFINED,
    EOF_TOKEN
} TokenType;

typedef std::pair<TokenType, std::string> Token;

std::string token_to_string(TokenType type);

#endif // GLOBAL_HPP