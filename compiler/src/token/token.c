#include <stdlib.h> // malloc()
#include <string.h> // strdup()

#include "lib.h"
#include "token.h"

Token* token_new(Str* value, I32 type)
{
    Token* new_token = malloc(sizeof(Token));
    new_token->value = strdup(str_to_char(value));
    str_free(value);
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
        case TOKEN_RET:
            return "TOKEN_RET";
            break;
        case TOKEN_GLB:
            return "TOKEN_GLB";
            break;
        case TOKEN_ID:
            return "TOKEN_ID";
            break;
        case TOKEN_NUM:
            return "TOKEN_NUM";
            break;
        case TOKEN_COMMA:
            return "TOKEN_COMMA";
            break;
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
            break;
        case TOKEN_LPAREN:
            return "TOKEN_LPAREN";
            break;
        case TOKEN_RPAREN:
            return "TOKEN_RPAREN";
            break;
        case TOKEN_LBRACE:
            return "TOKEN_LBRACE";
            break;
        case TOKEN_RBRACE:
            return "TOKEN_RBRACE";
            break;
        case TOKEN_AT:
            return "TOKEN_AT";
            break;
        case TOKEN_PERCENT:
            return "TOKEN_PERCENT";
            break;
        case TOKEN_AMPERSAND:
            return "TOKEN_AMPERSAND";
            break;
        case TOKEN_ASTERIX:
            return "TOKEN_ASTERIX";
            break;
        case TOKEN_SEMI:
            return "TOKEN_SEMI";
            break;
        case TOKEN_EOF:
            return "TOKEN_EOF";
            break;
        default:
            PANIC("in tokentype_to_string(): TokenType '%d' does not have string equivalent", tokentype);
            return NULL;
    }
}