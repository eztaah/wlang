#ifndef TOKEN_H
#define TOKEN_H

#include "lib.h"

typedef enum {
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MUL,
    TOKEN_COMMA,
    TOKEN_EQUAL,
    TOKEN_AT,
    TOKEN_PERCENT,
    TOKEN_AMPERSAND,
    TOKEN_PIPE,
    TOKEN_CARET,
    TOKEN_TILDE, 
    TOKEN_HASH,
    TOKEN_LESSTHAN,
    TOKEN_GREATERTHAN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMI,
    TOKEN_EOF,

    TOKEN_NOT_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_LESSTHAN_EQ,
    TOKEN_GREATERTHAN_EQ,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_LEFTSHIFT,
    TOKEN_RIGHTSHIFT,

    TOKEN_RET,
    TOKEN_GLB,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_LOOP,
    TOKEN_BREAK,

    TOKEN_ID,
    TOKEN_NUM,
} TokenType;

typedef struct {
    Char* value;
    TokenType type;
    I32 line;
} Token;

Token* token_new(Str* value, I32 type, I32 line);
Void token_free(Token* token);

Char* tokentype_to_string(TokenType tokentype);

Str* print_tokenlist(const List* token_list);

#endif