#ifndef TOKEN_H
#define TOKEN_H

#include "lib.h"

typedef enum {
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MUL,

    TOKEN_INT,
    TOKEN_MUT,
    TOKEN_ID,
    TOKEN_TYPE,

    TOKEN_COLON,
    TOKEN_EQUAL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_END_STATEMENT,
    TOKEN_EOF
} TokenType;

typedef struct {
    Char* value;
    TokenType type;
} Token;

Token* token_new(Str* value, I32 type);
Void token_free(Token* token);

Char* tokentype_to_string(TokenType tokentype);

#endif