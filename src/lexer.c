#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"


char *token_to_string(token_type type)
{
    switch (type) {
        case QUOTE:
            return "QUOTE";
        case CST:
            return "CST";
        case VAR:
            return "VAR";
        case NUMBER:
            return "NUMBER";
        case PLUS:
            return "PLUS";
        case BIN_AND:
            return "BIN_AND";
        case BIN_OR:
            return "BIN_OR";
        case XOR:
            return "XOR";
        case SHIFT_LEFT:
            return "SHIFT_LEFT";
        case SHIFT_RIGHT:
            return "SHIFT_RIGHT";
        case TIMES:
            return "TIMES";
        case MINUS:
            return "MINUS";
        case DIVIDE:
            return "DIVIDE";
        case MODULO:
            return "MODULO";
        case EQUALS:
            return "EQUALS";
        case EQUALS_EQUALS:
            return "EQUALS_EQUALS";
        case NOT_EQUALS:
            return "NOT_EQUALS";
        case LESS_THAN:
            return "LESS_THAN";
        case LESS_THAN_EQUALS:
            return "LESS_THAN_EQUALS";
        case GREATER_THAN:
            return "GREATER_THAN";
        case GREATER_THAN_EQUALS:
            return "GREATER_THAN_EQUALS";
        case OR:
            return "OR";
        case AND:
            return "AND";
        case LPAREN:
            return "LPAREN";
        case RPAREN:
            return "RPAREN";
        case LBRACE:
            return "LBRACE";
        case RBRACE:
            return "RBRACE";
        case COMMA:
            return "COMMA";
        case COLON:
            return "COLON";
        case TYPE:
            return "TYPE";
        case IF:
            return "IF";
        case WHILE:
            return "WHILE";
        case ELSE:
            return "ELSE";
        case IDENTIFIER:
            return "IDENTIFIER";
        case EOF_TOKEN:
            return "EOF_TOKEN";
        case END_LINE:
            return "END_LINE";
        default:
            //display_and_throw_internal_error("in token_to_string(): \'" + std::to_string(type) + "\' does not have string equivalent");
            exit(1);
    }
}

char* get_printable_lexer_output(Token *tokens, size_t num_tokens) {
    char *output = malloc(1024); // Initial buffer size, adjust as needed
    if (!output) return NULL;

    size_t output_size = 1024;
    size_t length = 0;
    int ident_level = 0;

    length += snprintf(output + length, output_size - length, "[\n");

    for (size_t i = 0; i < num_tokens; ++i) {
        Token token = tokens[i];
        char *token_type_name = token_to_string(token.type); // Implement this function based on your needs
        char indent[1024];
        memset(indent, ' ', ident_level * 4);
        indent[ident_level * 4] = '\0';

        char *format = "    (%d, %s, '%s'),\n";
        if (token.type == END_LINE) {
            if (i + 1 < num_tokens && tokens[i + 1].type == RBRACE) {
                format = "    (%d, %s, '%s'),\n";
            } else {
                format = "    (%d, %s, '%s'),\n\n";
            }
        } else if (token.type == RBRACE) {
            ident_level--;
            memset(indent, ' ', ident_level * 4);
            indent[ident_level * 4] = '\0';
            format = "    (%d, %s, '%s'),\n\n";
        }

        if (token.type == LBRACE) ident_level++;

        int needed = snprintf(NULL, 0, format, token.line_number, token_type_name, token.value);
        needed += snprintf(NULL, 0, "%s", indent);

        if (length + needed >= output_size) {
            output_size *= 2;
            output = realloc(output, output_size);
            if (!output) return NULL;
        }

        length += snprintf(output + length, output_size - length, "%s", indent);
        length += snprintf(output + length, output_size - length, format, token.line_number, token_type_name, token.value);
    }

    // Remove the last comma and newline
    if (length > 2) {
        output[length - 2] = '\0'; // Cuts off the last comma and newline
    }
    length += snprintf(output + length, output_size - length, "\n]");

    return output;
}

Token* lex(const char *code) {
    static Token tokens[] = {
        {3, IDENTIFIER, "variable"},
        {5, NUMBER, "123"},
        {7, EOF_TOKEN, ""}
    };
    return tokens;
}