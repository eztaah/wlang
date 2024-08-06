// #include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "lib/lib.h"
#include "token.h"

typedef struct {
    Char* src;
    size_t src_size;
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
    Char* value = calloc(2, sizeof(Char));
    value[0] = lexer->c;
    value[1] = '\0';

    Token* token = instanciate_token(value, type);

    return token;
}

static Token* lex_symbol(Lexer* lexer, I32 type)
{
    Char* value = calloc(2, sizeof(Char));
    value[0] = lexer->c;
    value[1] = '\0';

    Token* token = instanciate_token(value, type);

    lexer_advance(lexer);

    return token;
}

static Token* lex_number(Lexer* lexer)
{
    Char* value = calloc(1, sizeof(Char));

    while (isdigit(lexer->c)) {
        value = strcat_improved(value, (Char[]){lexer->c, 0});
        lexer_advance(lexer);
    }

    return instanciate_token(value, TOKEN_INT);
}

static Token* lex_word(Lexer* lexer)
{
    I32 token_type;
    Char* value = calloc(1, sizeof(Char));

    while (isalpha(lexer->c) || isdigit(lexer->c)) {
        value = strcat_improved(value, (Char[]){lexer->c, 0});
        lexer_advance(lexer);
    }

    if (strings_are_equals(value, "cst")) {
        token_type = TOKEN_MUT;
    }
    else if (strings_are_equals(value, "var")) {
        token_type = TOKEN_MUT;
    }
    else if (strings_are_equals(value, "I32")) {
        token_type = TOKEN_TYPE;
    }
    // for variables names
    else {
        token_type = TOKEN_ID;
    }

    return instanciate_token(value, token_type);
}

static Void skip_whitespace(Lexer* lexer)
{
    while (lexer->c == ' ' || lexer->c == '\t') {
        lexer_advance(lexer);
    }
}

static Token* lex_end_statement(Lexer* lexer)
{
    Char* value = calloc(2, sizeof(Char));
    value[0] = ' ';
    value[1] = '\0';

    Token* token = instanciate_token(value, TOKEN_END_STATEMENT);
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

    if (isalpha(lexer->c)) {
        return lex_word(lexer);
    }

    if (isdigit(lexer->c)) {
        return lex_number(lexer);
    }

    // handle end of statement
    if (lexer->c == 10 || lexer->c == 13) {
        return lex_end_statement(lexer);
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
        case '=':
            return lex_symbol(lexer, TOKEN_EQUAL);
        case '\0':
            break;
        default:
            printf("[Lexer]: Unexpected character `%c` (%d)\n", lexer->c, (I32)lexer->c);
            exit(1);
            break;
    }

    return NULL;
}

/**
 * This function create a lexer (you can see a lexer as a tool that contain data)
 */
static Lexer* instanciate_lexer(Char* src)
{
    Lexer* lexer = calloc(1, sizeof(Lexer));
    lexer->src = src;
    lexer->src_size = strlen(src);
    lexer->i = 0;
    lexer->c = src[lexer->i];

    return lexer;
}

static Void destroy_lexer(Lexer* lexer)
{
    free(lexer);
}

List* lex(Char* src)
{
    printf("Lexing...\n");

    Lexer* lexer = instanciate_lexer(src);
    List* token_list = init_list(sizeof(Token));

    while (lexer->c != '\0') {
        Token* token = lex_next_token(lexer);
        list_push(token_list, token);
    }

    // Add the EOF token to the array
    Token* token = lex_next_token_assumes(lexer, TOKEN_EOF);
    list_push(token_list, token);

    destroy_lexer(lexer);
    return token_list;
}
