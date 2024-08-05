#ifndef TOKEN_H
#define TOKEN_H

#include "lib/lib.h"

typedef enum {
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIV,
    TOKEN_MUL,

    TOKEN_INT,
    TOKEN_CST,
    TOKEN_VAR,
    TOKEN_ID,
    TOKEN_TYPE,

    TOKEN_COLON,
    TOKEN_EQUAL,

    TOKEN_END_STATEMENT,
    TOKEN_EOF
} TokenType;

typedef struct {
    Char* value;
    TokenType type;
} Token;

Char* token_to_string(Token token);
Token* instanciate_token(const Char* value, I32 type);

#endif