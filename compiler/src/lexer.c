#include <ctype.h>  // isalpha(), isdigit()
#include <stdlib.h> // exit()
#include <string.h> // strlen()

#include "compiler.h"

typedef struct {
    const Char* src;
    UX src_size;
    Char c;
    U32 i;
    I32 line;
} Lexer;

static Lexer* lexer_new(const Char* src)
{
    Lexer* lexer = safe_calloc(1, sizeof(Lexer));
    lexer->src = src;
    lexer->src_size = strlen(src);
    lexer->i = 0;
    lexer->c = src[lexer->i];
    lexer->line = 1;

    return lexer;
}

static Void lexer_free(Lexer* lexer)
{
    free(lexer);
}

static Void lexer_advance(Lexer* lexer)
{
    ASSERT(lexer->i < lexer->src_size, "lexer position should be < lexer->src_size");
    lexer->i += 1;

    lexer->c = lexer->src[lexer->i];

    if (lexer->c == '\n') {
        lexer->line += 1;
    }
}

static Void lexer_retreat(Lexer* lexer)
{
    if (lexer->i > 0) {
        lexer->i -= 1;
        lexer->c = lexer->src[lexer->i];
    }
    else {
        PANIC("Cannot retreat lexer; already at the start of input");
    }
}

static Token* lex_eof(Lexer* lexer, I32 type)
{
    Str* value = str_new_c(' ');
    Token* token = token_new(value, type, lexer->line);

    return token;
}

static Token* lex_symbol(Lexer* lexer, I32 type)
{
    Str* value = str_new_c(' ');
    Token* token = token_new(value, type, lexer->line);

    lexer_advance(lexer);

    return token;
}

static Token* lex_number(Lexer* lexer)
{
    Str* value = str_new("");

    // check for the 0x or 0b prefixes
    if (lexer->c == '0') {
        str_cat_c(value, lexer->c);
        lexer_advance(lexer);

        if (lexer->c == 'x' || lexer->c == 'X') { // hexadecimal
            str_cat_c(value, lexer->c);
            lexer_advance(lexer);

            while (isxdigit(lexer->c)) { // support 0-9, A-F, a-f
                str_cat_c(value, lexer->c);
                lexer_advance(lexer);
            }
        }
        else if (lexer->c == 'b' || lexer->c == 'B') { // binary
            str_cat_c(value, lexer->c);
            lexer_advance(lexer);

            while (lexer->c == '0' || lexer->c == '1') { // support only 0 and 1
                str_cat_c(value, lexer->c);
                lexer_advance(lexer);
            }
        }
        else { // decimal
            while (isdigit(lexer->c)) {
                str_cat_c(value, lexer->c);
                lexer_advance(lexer);
            }
        }
    }
    else { // if it's not a prefix, it's a normal decimal number
        while (isdigit(lexer->c)) {
            str_cat_c(value, lexer->c);
            lexer_advance(lexer);
        }
    }

    return token_new(value, TOKEN_NUM, lexer->line);
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
    if (str_cmp(value, "ret")) {
        token_type = TOKEN_RET;
    }
    else if (str_cmp(value, "glb")) {
        token_type = TOKEN_GLB;
    }
    else if (str_cmp(value, "if")) {
        token_type = TOKEN_IF;
    }
    else if (str_cmp(value, "else")) {
        token_type = TOKEN_ELSE;
    }
    else if (str_cmp(value, "loop")) {
        token_type = TOKEN_LOOP;
    }
    else if (str_cmp(value, "break")) {
        token_type = TOKEN_BREAK;
    }
    else {
        token_type = TOKEN_ID;
    }

    return token_new(value, token_type, lexer->line);
}

static Void skip_whitespace(Lexer* lexer)
{
    while (lexer->c == ' ' || lexer->c == '\t' || lexer->c == 13 || lexer->c == 10) {
        lexer_advance(lexer);
    }
}

static Token* lex_next_token(Lexer* lexer)
{
    ASSERT(lexer->c != '\0', "lexer->c should not be \\0");

    skip_whitespace(lexer);

    if (isalpha(lexer->c) || lexer->c == '_') {
        return lex_word(lexer);
    }

    if (isdigit(lexer->c)) {
        return lex_number(lexer);
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
        case '<':
            lexer_advance(lexer);
            if (lexer->c == '=') {
                return lex_symbol(lexer, TOKEN_LESSTHAN_EQ);
            }
            else if (lexer->c == '<') {
                return lex_symbol(lexer, TOKEN_LEFTSHIFT);
            }
            lexer_retreat(lexer);
            return lex_symbol(lexer, TOKEN_LESSTHAN);
        case '>':
            lexer_advance(lexer);
            if (lexer->c == '=') {
                return lex_symbol(lexer, TOKEN_GREATERTHAN_EQ);
            }
            else if (lexer->c == '>') {
                return lex_symbol(lexer, TOKEN_RIGHTSHIFT);
            }
            lexer_retreat(lexer);
            return lex_symbol(lexer, TOKEN_GREATERTHAN);
        case ',':
            return lex_symbol(lexer, TOKEN_COMMA);
        case '=':
            lexer_advance(lexer);
            if (lexer->c == '=') {
                return lex_symbol(lexer, TOKEN_EQUAL_EQUAL);
            }
            lexer_retreat(lexer);
            return lex_symbol(lexer, TOKEN_EQUAL);
        case '!':
            lexer_advance(lexer);
            if (lexer->c == '=') {
                return lex_symbol(lexer, TOKEN_NOT_EQUAL);
            }
            lexer_retreat(lexer);
            USER_PANIC(current_filename, lexer->line, "unexpected character after `!`: `%c` (ascii: %d)", lexer->c, (I32)lexer->c);
            return NULL;
        case '(':
            return lex_symbol(lexer, TOKEN_LPAREN);
        case ')':
            return lex_symbol(lexer, TOKEN_RPAREN);
        case '[':
            return lex_symbol(lexer, TOKEN_LBRACKET);
        case ']':
            return lex_symbol(lexer, TOKEN_RBRACKET);
        case '{':
            return lex_symbol(lexer, TOKEN_LBRACE);
        case '}':
            return lex_symbol(lexer, TOKEN_RBRACE);
        case '@':
            return lex_symbol(lexer, TOKEN_AT);
        case '%':
            return lex_symbol(lexer, TOKEN_PERCENT);
        case '&':
            lexer_advance(lexer);
            if (lexer->c == '&') {
                return lex_symbol(lexer, TOKEN_AND);
            }
            lexer_retreat(lexer);
            return lex_symbol(lexer, TOKEN_AMPERSAND);
        case '|':
            lexer_advance(lexer);
            if (lexer->c == '|') {
                return lex_symbol(lexer, TOKEN_OR);
            }
            lexer_retreat(lexer);
            return lex_symbol(lexer, TOKEN_PIPE);
        case '~':
            return lex_symbol(lexer, TOKEN_TILDE);
        case '#':
            return lex_symbol(lexer, TOKEN_HASH);
        case '^':
            return lex_symbol(lexer, TOKEN_CARET);
        case ';':
            return lex_symbol(lexer, TOKEN_SEMI);
        case '\0':
            return lex_eof(lexer, TOKEN_EOF);
        default:
            USER_PANIC(current_filename, lexer->line, "unexpected character `%c` (ascii: %d)", lexer->c, (I32)lexer->c);
            return NULL;
    }
}

List* lex(const Char* src)
{
    print(MSG_STEP, "lexing...\n");

    Lexer* lexer = lexer_new(src);
    List* token_list = list_new(sizeof(Token));

    while (lexer->c != '\0') {
        Token* token = lex_next_token(lexer);
        list_push(token_list, token);
    }

    lexer_free(lexer);
    return token_list;
}
