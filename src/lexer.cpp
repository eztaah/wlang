#include <cctype>
#include <iostream>
#include <set>
#include <stdexcept>

#include "lexer.hpp"

std::vector<Token> lex(const std::string &code)
{
    std::vector<Token> tokens;
    size_t i = 0;
    int line_number = 1;

    while (i < code.length()) {

        // Ignoring comments
        if (code[i] == '#') {
            while (i < code.length() && code[i] != '\n') {
                i++;
            }
            line_number++;
        }

        // Ignoring multi-lines comments
        else if (i + 2 < code.length() && code[i] == '"' && code[i + 1] == '"' && code[i + 2] == '"') {
            i += 3;
            line_number++;
            while (i + 2 < code.length() && !(code[i] == '"' && code[i + 1] == '"' && code[i + 2] == '"')) {
                i++;
                if (code[i] == '\n') {
                    line_number++;
                }
            }
            if (i + 2 < code.length()) {
                i += 3;
            }
        }

        // handle \n
        else if (code[i] == '\n') {

            // Vérifiez le token précédent pour décider si un point-virgule implicite est nécessaire
            if (!tokens.empty() && tokens.back().type != LBRACE && tokens.back().type != SEMICOLON && tokens.back().type != RBRACE) {
                // Ajoutez un point-virgule seulement si le dernier token n'était pas une accolade ouvrante,
                // une accolade fermante ou déjà un point-virgule
                tokens.push_back(Token(SEMICOLON, "", line_number));
            }

            // Passer les sauts de ligne supplémentaires
            while (i + 1 < code.length() && code[i + 1] == '\n') {
                i++;
                line_number++;
            }

            line_number++;
            i++;
        }

        // Handling numbers and minus operator
        else if (code[i] == '-') {
            if (i + 1 < code.length() && isdigit(code[i + 1])) {
                std::string num = "-";
                i++;
                while (i < code.length() && isdigit(code[i])) {
                    num += code[i];
                    i++;
                }
                tokens.push_back(Token(NUMBER, num, line_number));
            }
            else {
                tokens.push_back(Token(MINUS, "", line_number));
                i++;
            }
        }
        else if (isdigit(code[i])) {
            std::string num;
            while (i < code.length() && isdigit(code[i])) {
                num += code[i];
                i++;
            }
            tokens.push_back(Token(NUMBER, num, line_number));
        }

        // Handling operators
        else if (code[i] == '+') {
            tokens.push_back(Token(PLUS, "", line_number));
            i++;
        }
        else if (code[i] == '*') {
            tokens.push_back(Token(TIMES, "", line_number));
            i++;
        }
        else if (code[i] == '/' && code[i + 1] == '/') {
            tokens.push_back(Token(DIVIDE, "", line_number));
            i += 2;
        }
        else if (code[i] == '%') {
            tokens.push_back(Token(MODULO, "", line_number));
            i++;
        }
        else if (code[i] == '&') {
            tokens.push_back(Token(BIN_AND, "", line_number));
            i++;
        }
        else if (code[i] == '|') {
            tokens.push_back(Token(BIN_OR, "", line_number));
            i++;
        }
        else if (code[i] == '^') {
            tokens.push_back(Token(XOR, "", line_number));
            i++;
        }

        // Handling =, ==, >, <=, etc
        else if (code[i] == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(EQUALS_EQUALS, "", line_number));
                i += 2;
            }
            else {
                tokens.push_back(Token(EQUALS, "", line_number));
                i++;
            }
        }
        else if (code[i] == '!' && code[i + 1] == '=') {
            tokens.push_back(Token(NOT_EQUALS, "", line_number));
            i += 2;
        }
        else if (code[i] == '<') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(LESS_THAN_EQUALS, "", line_number));
                i += 2;
            }
            else if (i + 1 < code.length() && code[i + 1] == '<') {
                tokens.push_back(Token(SHIFT_LEFT, "", line_number));
                i += 2;
            }
            else {
                tokens.push_back(Token(LESS_THAN, "", line_number));
                i++;
            }
        }
        else if (code[i] == '>') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(GREATER_THAN_EQUALS, "", line_number));
                i += 2;
            }
            else if (i + 1 < code.length() && code[i + 1] == '>') {
                tokens.push_back(Token(SHIFT_RIGHT, "", line_number));
                i += 2;
            }
            else {
                tokens.push_back(Token(GREATER_THAN, "", line_number));
                i++;
            }
        }

        // Handling symbols
        else if (code[i] == ';') {
            tokens.push_back(Token(SEMICOLON, "", line_number));
            i++;
        }
        else if (code[i] == '(') {
            tokens.push_back(Token(LPAREN, "", line_number));
            i++;
        }
        else if (code[i] == ')') {
            tokens.push_back(Token(RPAREN, "", line_number));
            i++;
        }
        else if (code[i] == '{') {
            tokens.push_back(Token(LBRACE, "", line_number));
            i++;
        }
        else if (code[i] == '}') {
            tokens.push_back(Token(RBRACE, "", line_number));
            i++;
        }
        else if (code[i] == ',') {
            tokens.push_back(Token(COMMA, "", line_number));
            i++;
        }
        else if (code[i] == ':') {
            tokens.push_back(Token(COLON, "", line_number));
            i++;
        }

        // Handling strings
        else if (code[i] == '"') {
            tokens.push_back(Token(QUOTE, "", line_number));
            i++;
            std::string res = "";
            while (code[i] != '"') {
                res += code[i];
                i++;
            }
            tokens.push_back(Token(IDENTIFIER, res, line_number));
            tokens.push_back(Token(QUOTE, "", line_number));
            i++;
        }

        // Handling variables and keywords
        else if (isalpha(code[i])) {
            std::string ident;
            while (i < code.length() && (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_')) {
                ident += code[i];
                i++;
            }
            // Handling keywords
            if (ident == "cst") {
                tokens.push_back(Token(CST, "", line_number));
            }
            else if (ident == "var") {
                tokens.push_back(Token(VAR, "", line_number));
            }
            else if (ident == "if") {
                tokens.push_back(Token(IF, "", line_number));
            }
            else if (ident == "while") {
                tokens.push_back(Token(WHILE, "", line_number));
            }
            else if (ident == "else") {
                tokens.push_back(Token(ELSE, "", line_number));
            }
            else if (ident == "i64") {
                tokens.push_back(Token(TYPE, "i64", line_number));
            }

            // handling var names
            else {
                tokens.push_back(Token(IDENTIFIER, ident, line_number));
            }
        }

        // Ignoring spaces, tabs, new lines
        else if (code[i] == ' ' || code[i] == '\t') {
            i++;
        }
        // Error otherwise
        else {
            display_and_throw_error("character '" + std::to_string(code[i]) + "' (ASCII: " + std::to_string(static_cast<int>(code[i])) + ") is not valid",
                                    line_number);
            exit(1);
        }
    }
    // Adding end-of-file character
    tokens.push_back(Token(EOF_TOKEN, "", line_number));
    return tokens;
}

std::string get_printable_lexer_output(const std::vector<Token> &tokens)
{
    std::string output;
    int indentLevel = 0;
    output += "[\n";

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token &token = tokens[i];
        std::string tokenTypeName = token_to_string(token.type);
        std::string indent = std::string(indentLevel * 4, ' '); // 4 spaces per indentation

        if (token.type == SEMICOLON) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == RBRACE) {
                output += indent + "    ('" + tokenTypeName + "', '" + token.value + "', '" + std::to_string(token.line_number) + "'),\n";
            }
            else {
                output += indent + "    ('" + tokenTypeName + "', '" + token.value + "', '" + std::to_string(token.line_number) + "'),\n\n";
            }
        }
        else if (token.type == RBRACE) {
            indentLevel--;
            indent = std::string(indentLevel * 4, ' ');

            output += indent + "    ('" + tokenTypeName + "', '" + token.value + "', '" + std::to_string(token.line_number) + "'),\n\n";
        }
        else {
            // classic tokens
            output += indent + "    ('" + tokenTypeName + "', '" + token.value + "', '" + std::to_string(token.line_number) + "'),\n";
        }

        if (token.type == LBRACE) {
            indentLevel++; // valable pour le prochain token
        }
    }
    // remove last comma and \n
    if (!tokens.empty()) {
        output.pop_back(); // remove \n
        output.pop_back(); // remove last comma
    }
    output += "\n]";
    return output;
}