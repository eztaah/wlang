#ifndef TOKEN_H
#define TOKEN_H

typedef struct TOKEN_STRUCT {
    Char* value;
    enum {
        TOKEN_INT,
        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_DIV,
        TOKEN_MUL,
        TOKEN_EOF,
    } type;
} Token;

Char* token_to_string(Token token);
Token* instanciate_token(const Char* value, I32 type);

#endif