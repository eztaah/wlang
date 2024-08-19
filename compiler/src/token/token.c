#include <stdlib.h> // safe_malloc()
#include <string.h> // strdup()

#include "lib.h"
#include "token.h"

Token* token_new(Str* value, I32 type, I32 line)
{
    Token* new_token = safe_malloc(sizeof(Token));
    new_token->value = strdup(str_to_char(value));
    new_token->line = line;
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
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
        case TOKEN_DIV:
            return "TOKEN_DIV";
        case TOKEN_MUL:
            return "TOKEN_MUL";
        case TOKEN_COMMA:
            return "TOKEN_COMMA";
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
        case TOKEN_AT:
            return "TOKEN_AT";
        case TOKEN_PERCENT:
            return "TOKEN_PERCENT";
        case TOKEN_AMPERSAND:
            return "TOKEN_AMPERSAND";
        case TOKEN_PIPE:
            return "TOKEN_PIPE";
        case TOKEN_CARET:
            return "TOKEN_CARET";
        case TOKEN_LESSTHAN:
            return "TOKEN_LESSTHAN";
        case TOKEN_GREATERTHAN:
            return "TOKEN_GREATERTHAN";
        case TOKEN_LBRACKET:
            return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:
            return "TOKEN_RBRACKET";
        case TOKEN_LPAREN:
            return "TOKEN_LPAREN";
        case TOKEN_RPAREN:
            return "TOKEN_RPAREN";
        case TOKEN_LBRACE:
            return "TOKEN_LBRACE";
        case TOKEN_RBRACE:
            return "TOKEN_RBRACE";
        case TOKEN_SEMI:
            return "TOKEN_SEMI";
        case TOKEN_EOF:
            return "TOKEN_EOF";

        case TOKEN_NOT_EQUAL:
            return "TOKEN_NOT_EQUAL";
        case TOKEN_EQUAL_EQUAL:
            return "TOKEN_EQUAL_EQUAL";
        case TOKEN_LESSTHAN_EQ:
            return "TOKEN_LESSTHAN_EQ";
        case TOKEN_GREATERTHAN_EQ:
            return "TOKEN_GREATERTHAN_EQ";
        case TOKEN_AND:
            return "TOKEN_AND";
        case TOKEN_OR:
            return "TOKEN_OR";
        case TOKEN_LEFTSHIFT:
            return "TOKEN_LEFTSHIFT";
        case TOKEN_RIGHTSHIFT:
            return "TOKEN_RIGHTSHIFT";

        case TOKEN_RET:
            return "TOKEN_RET";
        case TOKEN_GLB:
            return "TOKEN_GLB";
        case TOKEN_IF:
            return "TOKEN_IF";
        case TOKEN_ELSE:
            return "TOKEN_ELSE";
        case TOKEN_LOOP:
            return "TOKEN_LOOP";
        case TOKEN_BREAK:
            return "TOKEN_BREAK";

        case TOKEN_ID:
            return "TOKEN_ID";
        case TOKEN_NUM:
            return "TOKEN_NUM";

        default:
            PANIC("in tokentype_to_string(): TokenType '%d' does not have string equivalent", tokentype);
            return NULL;
    }
}
