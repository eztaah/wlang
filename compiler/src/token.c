#include "token.h"
#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token* token_new(const Char* value, I32 type)
{
    Token* new_token = malloc(sizeof(Token));
    new_token->value = strdup(value);
    new_token->type = type;
    return new_token;
}

Void token_free(Token* token)
{
    free(token);
}

Char* tokentype_to_string(TokenType tokentype)
{
    switch (tokentype) {
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
        case TOKEN_INT:
            return "TOKEN_INT";
            break;
        case TOKEN_MUT:
            return "TOKEN_MUT";
            break;
        case TOKEN_ID:
            return "TOKEN_ID";
            break;
        case TOKEN_TYPE:
            return "TOKEN_TYPE";
            break;
        case TOKEN_COLON:
            return "TOKEN_COLON";
            break;
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
            break;
        case TOKEN_END_STATEMENT:
            return "TOKEN_END_STATEMENT";
            break;
        case TOKEN_EOF:
            return "TOKEN_EOF";
            break;
        default:
            PANIC("in tokentype_to_string(): TokenType '%d' does not have string equivalent", tokentype);
            return NULL;
    }
}