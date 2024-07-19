// #include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "lib/lib.h"
#include "token.h"

typedef struct {
    Char* src;
    // size_t src_size;
    Char c;
    U32 i;
} Lexer;

static Void lexer_advance(Lexer* lexer)
{
    lexer->i += 1;

    ASSERT(lexer->i < strlen(lexer->src), "we should not be here");
    lexer->c = lexer->src[lexer->i];
}

static Token* lex_next_token_assumes(Lexer* lexer, I32 type)
{
    Char* value = calloc(2, sizeof(Char));
    value[0] = lexer->c;
    value[1] = '\0';

    Token* token = instanciate_token(value, type);

    lexer_advance(lexer);

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
        strcat_improved(value, (Char[]){lexer->c, 0});
        lexer_advance(lexer);
    }

    return instanciate_token(value, TOKEN_INT);
}

static Void skip_whitespace(Lexer* lexer)
{
    while (lexer->c == 13 || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\t') {
        lexer_advance(lexer);
    }
}

static Token* lex_next_token(Lexer* lexer)
{
    ASSERT(lexer->c != '\0', "lexer->c should not be \\0");

    skip_whitespace(lexer);

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
        case '\0': 
            break;
        default: 
            printf("[Lexer]: Unexpected character `%c` (%d)\n", lexer->c, (int)lexer->c); 
            exit(1); 
            break;
    }
}

static Void destroy_lexer(Lexer* lexer)
{
    free(lexer);
}

/**
 * This function create a lexer (you can see a lexer as a tool that contain data)
 */
static Lexer* instanciate_lexer(Char* src)
{
    Lexer* lexer = calloc(1, sizeof(Lexer));
    lexer->src = src;
    // lexer->src_size = strlen(src);
    lexer->i = 0;
    lexer->c = src[lexer->i];

    return lexer;
}

/**
 * This function will return a list with all the tokens 
 */
Token** lex(Char* src) {
    Lexer* lexer = instanciate_lexer(src);
    List* token_list = init_list(sizeof(Token));

    while (lexer->c != '\0') {
        Token* token = lex_next_token(lexer);
        list_push(token_list, token);
    }

    // Add the EOF token to the array
    Token* token = lex_next_token_assumes(lexer, TOKEN_EOF);
    list_push(token_list, token);

    destroy_lexer(token);
    return token_list;
}



//////////////////////////////////////////////////////////////////////////////////////////
// List* lex_old(Char* src)
// {
//     I32 i = 0;
//     I32 src_length = strlen(src);
//     List* token_list = init_list(sizeof(Token));

//     while (i < src_length) {

//         // ignoring spaces, tabs, new lines
//         if (src[i] == ' ' || src[i] == '\t') {
//             i++;
//         }

//         else if (src[i] == '\n') {
//             i++;
//         }

//         // handling numbers
//         else if (isdigit(src[i])) {
//             Char num[256] = ""; // il faut allouer plus de memoire pour num car on va ajouter des nombres au fur et a mesure
//             while (i < src_length && isdigit(src[i])) {
//                 Char temp[2] = {src[i], '\0'};
//                 strcat(num, temp);
//                 i++;
//             }
//             printf("creation TOKEN_INT");
//             list_push(token_list, create_token(num, TOKEN_INT));
//         }

//         // handling operators
//         else if (src[i] == '+') {
//             printf("creation TOKEN_PLUS");
//             list_push(token_list, create_token(" ", TOKEN_PLUS));
//             i++;
//         }
//         else if (src[i] == '-') {
//             printf("creation TOKEN_MINUS");
//             list_push(token_list, create_token(" ", TOKEN_MINUS));
//             i++;
//         }
//         else if (src[i] == '*') {
//             printf("creation TOKEN_MUL");
//             list_push(token_list, create_token(" ", TOKEN_MUL));
//             i++;
//         }
//         else if (src[i] == '/' && src[i + 1] == '/') {
//             printf("creation TOKEN_DIV");
//             list_push(token_list, create_token(" ", TOKEN_DIV));
//             i += 2;
//         }
//     }

//     list_push(token_list, create_token(" ", TOKEN_EOF));

//     return token_list;
// }

Char* convert_lexer_output_to_char(List* lexer_output)
{
    Char* output = init_empty_string();

    for (I32 i = 0; i < lexer_output->size; i++) {
        const Token* token = (Token*)lexer_output->items[i];
        strcat_improved(output, token_to_string(*token));
        strcat_improved(output, ", ");
    }

    return output;
}

// Char*convert_lexer_output_to_char(List *lexer_output)
// {
//     // Calculer la taille totale nécessaire
//     size_t total_length = 1; // Pour le caractère nul de fin
//     for (size_t i = 0; i < lexer_output->size; i++) {
//         Token *token = (Token*)lexer_output->items[i];
//         total_length += strlen(token_to_string(*token)) + 2; // Ajouter 2 pour ", "
//     }

//     // Allouer la mémoire pour la chaîne de sortie
//     Char*output = malloc(total_length);
//     if (!output) {
//         perror("Unable to allocate memory for lexer output string");
//         exit(EXIT_FAILURE);
//     }
//     output[0] = '\0'; // Initialiser la chaîne vide

//     // Construire la chaîne de sortie
//     for (size_t i = 0; i < lexer_output->size; i++) {
//         Token *token = (Token*)lexer_output->items[i];
//         strcat(output, token_to_string(*token));
//         if (i < lexer_output->size - 1) {
//             strcat(output, ", ");
//         }
//     }

//     return output;
// }
