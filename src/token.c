#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "token.h"

Token* instanciate_token(const Char* value, I32 type)
{
    Token* new_token = malloc(sizeof(Token));
    new_token->value = strdup(value);
    new_token->type = type;
    return new_token;
}

Char* token_to_string(Token token)
{
    switch (token.type) {
        case TOKEN_INT:
            return "TOKEN_INT";
            break;
        case TOKEN_PLUS:
            return "TOKEN_PLUS";
            break;
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
            break;
        case TOKEN_DIV:
            return "TOKEN_DIV";
            break;
        case TOKEN_MUL:
            return "TOKEN_MUL";
            break;
        case TOKEN_EOF:
            return "TOKEN_EOF";
            break;
        default:
            printf("in token_to_string(): a token does not have string equivalent");
            abort();
    }
}