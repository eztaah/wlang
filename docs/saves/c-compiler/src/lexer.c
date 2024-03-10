#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


Token* lexer(char *code, int *tokenCount) {

    Token *tokens = malloc(LEXER_OUTPUT_SIZE * sizeof(Token));

    char *currentChar;

    for (currentChar = code; *currentChar != '\0'; currentChar++) {

        // Handling numbers
        if (isdigit(*currentChar)) {
            char *start = currentChar;
            while (isdigit(*currentChar)) currentChar++;
            addToken(&tokens, tokenCount, NUMBER, strndup(start, currentChar - start));
            currentChar--; // Adjust because of the for loop increment
        } 

        // Handling operators
        else if (*currentChar == '+') {
            addToken(&tokens, tokenCount, PLUS, NULL);
        } 
        else if (*currentChar == '*') {
            addToken(&tokens, tokenCount, TIMES, NULL);
        }
        else if (*currentChar == '=') {
            if (*(currentChar + 1) == '=') {
                addToken(&tokens, tokenCount, EQUALS_EQUALS, NULL);
                currentChar++;
            } else {
                addToken(&tokens, tokenCount, EQUALS, NULL);
            }
        }
        else if (*currentChar == ';') {
            addToken(&tokens, tokenCount, SEMICOLON, NULL);
        } 
        
        // Handling parentheses and braces
        else if (*currentChar == '(') {
            addToken(&tokens, tokenCount, LPAREN, NULL);
        } 
        else if (*currentChar == ')') {
            addToken(&tokens, tokenCount, RPAREN, NULL);
        } 
        else if (*currentChar == '{') {
            addToken(&tokens, tokenCount, LBRACE, NULL);
        } 
        else if (*currentChar == '}') {
            addToken(&tokens, tokenCount, RBRACE, NULL);
        } 

        // Handling variables and keywords
        else if (isalpha(*currentChar)) {
            char *start = currentChar;
            while (isalpha(*currentChar) || isdigit(*currentChar) || *currentChar == '_') currentChar++;
            char *ident = strndup(start, currentChar - start);
            if (strcmp(ident, "if") == 0) {
                addToken(&tokens, tokenCount, IF, NULL);
            } else {
                addToken(&tokens, tokenCount, IDENTIFIER, ident);
            }
            free(ident);
            currentChar--; // Adjust because of the for loop increment
        } 

        // Ignoring spaces, tabs, new lines
        else if (*currentChar == ' ' || *currentChar == '\n' || *currentChar == '\t') {
            // Ignore whitespace
        } 

        // Error otherwise
        else {
            printf("[!] Unknown character: %c\n", *currentChar);
            exit(1);
        }
    }
    addToken(&tokens, tokenCount, EOF_TOKEN, NULL);

    return tokens;
}


// Adds a token to the list
void addToken(Token **tokens, int *tokenCount, TokenType type, char *value) {
    if (*tokenCount >= LEXER_OUTPUT_SIZE) {
        // Maximum capacity reached, can no longer add tokens
        fprintf(stderr, "Error: Maximum token capacity (%d) reached.\n", LEXER_OUTPUT_SIZE);
        return;
    }

    // Adding the token type
    (*tokens)[*tokenCount].type = type;

    // Adding the token value, if provided
    if (value) {
        (*tokens)[*tokenCount].value = strdup(value);
    } else {
        (*tokens)[*tokenCount].value = NULL;
    }

    // Incrementing the token counter
    (*tokenCount)++;
}


void pretty_print(Token *tokens, const int *tokenCount) {
    printf("\n---- LEXER OUTPUT ----\n");
    for (int i = 0; i < *tokenCount; i++) {
        switch (tokens[i].type) {
            case IDENTIFIER:
                printf("> IDENTIFIER : %s\n", tokens[i].value);
                break;
            case EQUALS:
                printf("> EQUALS\n");
                break;
            case NUMBER:
                printf("> NUMBER : %s\n", tokens[i].value);
                break;
            case PLUS:
                printf("> PLUS\n");
                break;
            case TIMES:
                printf("> TIMES\n");
                break;
            case SEMICOLON:
                printf("> SEMICOLON\n\n");
                break;
            case IF:
                printf("> IF\n");
                break;
            case LPAREN:
                printf("> LPAREN\n");
                break;
            case EQUALS_EQUALS:
                printf("> EQUALS_EQUALS\n");
                break;
            case RPAREN:
                printf("> RPAREN\n");
                break;
            case LBRACE:
                printf("> LBRACE\n");
                break;
            case RBRACE:
                printf("> RBRACE\n");
                break;
            case EOF_TOKEN:
                printf("> EOF\n");
                break;
            default:
                printf("> [!] The pretty print function doesn't know this token\n");
        }
    }
}
