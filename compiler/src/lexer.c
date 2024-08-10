// #include <stdlib.h>
#include "compiler.h"
#include "lib.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Char* src;
    UX src_size;
    Char c;
    U32 i;
} Lexer;

static Void lexer_advance(Lexer* lexer)
{
    ASSERT(lexer->i < lexer->src_size, "lexer position should be < lexer->src_size");
    lexer->i += 1;

    lexer->c = lexer->src[lexer->i];
}

static Token* lex_next_token_assumes(Lexer* lexer, I32 type)
{
    Str* value = str_new_c(lexer->c);
    Token* token = token_new(value, type);

    return token;
}

static Token* lex_symbol(Lexer* lexer, I32 type)
{
    Str* value = str_new_c(lexer->c);
    Token* token = token_new(value, type);

    lexer_advance(lexer);

    return token;
}

static Token* lex_number(Lexer* lexer)
{
    Str* value = str_new("");

    while (isdigit(lexer->c)) {
        str_cat_c(value, lexer->c);
        lexer_advance(lexer);
    }

    return token_new(value, TOKEN_INT);
}

static Token* lex_word(Lexer* lexer)
{
    Str* value = str_new("");

    while (isalpha(lexer->c) || isdigit(lexer->c) || lexer->c == '_') {
        str_cat_c(value, lexer->c);
        lexer_advance(lexer);
    }

    I32 token_type;
    // handle keywords
    if (str_cmp(value, "cst")) {
        token_type = TOKEN_MUT;
    }
    else if (str_cmp(value, "var")) {
        token_type = TOKEN_MUT;
    }
    else if (str_cmp(value, "fun")) {
        token_type = TOKEN_FUN;
    }
    else if (str_cmp(value, "return")) {
        token_type = TOKEN_RETURN;
    }

    // handle types
    else if (str_cmp(value, "I64")) {
        token_type = TOKEN_TYPE;
    }
    else if (str_cmp(value, "Void")) {
        token_type = TOKEN_TYPE;
    }
    // handle variable variables names
    else {
        token_type = TOKEN_ID;
    }

    return token_new(value, token_type);
}

static Void skip_whitespace(Lexer* lexer)
{
    while (lexer->c == ' ' || lexer->c == '\t' || lexer->c == 13 || lexer->c == 10) {
        lexer_advance(lexer);
    }
}

static void lexer_skip_comment(Lexer* lexer)
{
    while (lexer->c == '#') {
        while (lexer->c != '\n' && lexer->c != '\0') {
            lexer_advance(lexer);
        }
        if (lexer->c == '\n') {
            lexer_advance(lexer);
        }
        skip_whitespace(lexer);
    }
    skip_whitespace(lexer);
}

static Token* lex_end_instruction(Lexer* lexer)
{
    Str* value = str_new(";");

    Token* token = token_new(value, TOKEN_END_INSTR);
    lexer_advance(lexer);

    // handle other \n or carriage return
    while (lexer->c == 10 || lexer->c == 13) {
        lexer_advance(lexer);
    }

    return token;
}

static Token* lex_next_token(Lexer* lexer)
{
    ASSERT(lexer->c != '\0', "lexer->c should not be \\0");

    skip_whitespace(lexer);
    lexer_skip_comment(lexer);

    if (isalpha(lexer->c) || lexer->c == '_') {
        return lex_word(lexer);
    }

    if (isdigit(lexer->c)) {
        return lex_number(lexer);
    }

    // handle end of statement
    if (lexer->c == ';') {
        return lex_end_instruction(lexer);
    }

    switch (lexer->c) {
        case '+':
            return lex_symbol(lexer, TOKEN_PLUS);
        case '-':
            return lex_symbol(lexer, TOKEN_MINUS);
        case '/':
            return lex_symbol(lexer, TOKEN_DIV);
        case '*':
            return lex_symbol(lexer, TOKEN_MUL);
        case ':':
            return lex_symbol(lexer, TOKEN_COLON);
        case ',':
            return lex_symbol(lexer, TOKEN_COMMA);
        case '=':
            return lex_symbol(lexer, TOKEN_EQUAL);
        case '(':
            return lex_symbol(lexer, TOKEN_LPAREN);
        case ')':
            return lex_symbol(lexer, TOKEN_RPAREN);
        case '{':
            return lex_symbol(lexer, TOKEN_LBRACE);
        case '}':
            return lex_symbol(lexer, TOKEN_RBRACE);
        case '@':
            return lex_symbol(lexer, TOKEN_AT);
        case '&':
            return lex_symbol(lexer, TOKEN_AMPERSAND);
        case '\0':
            return lex_next_token_assumes(lexer, TOKEN_EOF);
        default:
            print(MSG_ERROR,"[Lexer]: Unexpected character `%c` (%d)\n", lexer->c, (I32)lexer->c);
            exit(EXIT_FAILURE);
            break;
    }
}

/**
 * This function create a lexer (you can see a lexer as a tool that contain data)
 */
static Lexer* lexer_new(Char* src)
{
    Lexer* lexer = calloc(1, sizeof(Lexer));
    lexer->src = src;
    lexer->src_size = strlen(src);
    lexer->i = 0;
    lexer->c = src[lexer->i];

    return lexer;
}

static Void lexer_free(Lexer* lexer)
{
    free(lexer);
}

List* lex(Char* src)
{
    print(MSG_STEP, "lexing...\n");

    Lexer* lexer = lexer_new(src);
    List* token_list = list_new(sizeof(Token));

    while (lexer->c != '\0') {
        Token* token = lex_next_token(lexer);
        list_push(token_list, token);
    }

    // Add the EOF token to the array
    Token* token = lex_next_token_assumes(lexer, TOKEN_EOF);
    list_push(token_list, token);

    lexer_free(lexer);
    return token_list;
}
