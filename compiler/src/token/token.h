#ifndef TOKEN_H
#define TOKEN_H

#include "lib.h"

typedef enum {
    TOKEN_ID,
    TOKEN_NUM,

    TOKEN_RET,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MUL,

    TOKEN_COMMA,
    TOKEN_EQUAL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_AT,
    TOKEN_AMPERSAND,

    TOKEN_SEMI,
    TOKEN_EOF
} TokenType;

typedef struct {
    Char* value;
    TokenType type;
} Token;

Token* token_new(Str* value, I32 type);
Void token_free(Token* token);

Char* tokentype_to_string(TokenType tokentype);

Str* print_tokenlist(const List* token_list);

#endif