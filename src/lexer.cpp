#include <cctype>
#include <iostream>
#include <set>
#include <stdexcept>

#include "lexer.hpp"

std::vector<Token> lex(const std::string &code)
{
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < code.length()) {

        // Ignoring comments
        if (code[i] == '#') {
            while (i < code.length() && code[i] != '\n') {
                i++;
            }
        }

        // Ignoring multi-lines comments
        else if (i + 2 < code.length() && code[i] == '"' && code[i + 1] == '"' && code[i + 2] == '"') {
            i += 3;
            while (i + 2 < code.length() && !(code[i] == '"' && code[i + 1] == '"' && code[i + 2] == '"')) {
                i++;
            }
            if (i + 2 < code.length()) {
                i += 3;
            }
        }

        // Handling numbers
        else if (isdigit(code[i])) {
            std::string num;
            while (i < code.length() && isdigit(code[i])) {
                num += code[i];
                i++;
            }
            tokens.push_back(Token(NUMBER, num));
        }

        // Handling operators
        else if (code[i] == '+') {
            tokens.push_back(Token(PLUS, ""));
            i++;
        }
        else if (code[i] == '*') {
            tokens.push_back(Token(TIMES, ""));
            i++;
        }
        else if (code[i] == '-') {
            tokens.push_back(Token(MINUS, ""));
            i++;
        }
        else if (code[i] == '/' && code[i + 1] == '/') {
            tokens.push_back(Token(DIVIDE, ""));
            i += 2;
        }
        else if (code[i] == '&') {
            // if (i + 1 < code.length() && code[i + 1] == '&') {
            //     tokens.push_back(Token(AND, ""));
            //     i += 2;
            // }
            // else {
            // }
            tokens.push_back(Token(BIN_AND, ""));
            i++;
        }
        else if (code[i] == '|') {
            // if (i + 1 < code.length() && code[i + 1] == '|') {
            //     tokens.push_back(Token(OR, ""));
            //     i += 2;
            // }
            // else {

            // }
            tokens.push_back(Token(BIN_OR, ""));
            i++;
        }
        else if (code[i] == '^') {
            tokens.push_back(Token(XOR, ""));
            i++;
        }

        // Handling =, ==, >, <=, etc
        else if (code[i] == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(EQUALS_EQUALS, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(EQUALS, ""));
                i++;
            }
        }
        else if (code[i] == '!' && code[i + 1] == '=') {
            tokens.push_back(Token(NOT_EQUALS, ""));
            i += 2;
        }
        else if (code[i] == '<') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(LESS_THAN_EQUALS, ""));
                i += 2;
            }
            else if (i + 1 < code.length() && code[i + 1] == '<') {
                tokens.push_back(Token(SHIFT_LEFT, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(LESS_THAN, ""));
                i++;
            }
        }
        else if (code[i] == '>') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(GREATER_THAN_EQUALS, ""));
                i += 2;
            }
            else if (i + 1 < code.length() && code[i + 1] == '>') {
                tokens.push_back(Token(SHIFT_RIGHT, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(GREATER_THAN, ""));
                i++;
            }
        }

        // Handling symbols
        else if (code[i] == ';') {
            tokens.push_back(Token(SEMICOLON, ""));
            i++;
        }
        else if (code[i] == '(') {
            tokens.push_back(Token(LPAREN, ""));
            i++;
        }
        else if (code[i] == ')') {
            tokens.push_back(Token(RPAREN, ""));
            i++;
        }
        else if (code[i] == '{') {
            tokens.push_back(Token(LBRACE, ""));
            i++;
        }
        else if (code[i] == '}') {
            tokens.push_back(Token(RBRACE, ""));
            i++;
        }
        else if (code[i] == ',') {
            tokens.push_back(Token(COMMA, ""));
            i++;
        }

        // Handling strings
        else if (code[i] == '"') {
            tokens.push_back(Token(QUOTE, ""));
            i++;
            std::string res = "";
            while (code[i] != '"') {
                res += code[i];
                i++;
            }
            tokens.push_back(Token(IDENTIFIER, res));
            tokens.push_back(Token(QUOTE, ""));
            i++;
        }

        // Handling variables and keywords
        else if (isalpha(code[i])) {
            std::string ident;
            while (i < code.length() &&
                   (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_')) {
                ident += code[i];
                i++;
            }
            // Handling keywords
            if (ident == "cst") {
                tokens.push_back(Token(CST, ""));
            }
            else if (ident == "var") {
                tokens.push_back(Token(VAR, ""));
            }
            else if (ident == "if") {
                tokens.push_back(Token(IF, ""));
            }
            else if (ident == "while") {
                tokens.push_back(Token(WHILE, ""));
            }
            else if (ident == "else") {
                tokens.push_back(Token(ELSE, ""));
            }

            // handling var names
            else {
                tokens.push_back(Token(IDENTIFIER, ident));
            }
        }

        // Ignoring spaces, tabs, new lines
        else if (code[i] == ' ' || code[i] == '\n' || code[i] == '\t') {
            i++;
        }
        // Error otherwise
        else {
            std::cout << "\033[31m[!] Lexer error : Unknown character: '" << code[i] << "' (ASCII: " << static_cast<int>(code[i]) << ")\033[0m" << std::endl;
            exit(-1);
        }
    }
    // Adding end-of-file character
    tokens.push_back(Token(EOF_TOKEN, ""));
    return tokens;
}

std::string get_printable_lexer_output(const std::vector<Token> &tokens)
{
    std::string output;
    output += "[\n";
    for (const Token &token : tokens) {
        std::string tokenTypeName = token_to_string(token.first);
        output += "    ('" + tokenTypeName + "', '" + token.second + "'),\n";
        if (token.first == SEMICOLON) {
            output += "\n";
        }
    }
    output += "]";
    return output;
}