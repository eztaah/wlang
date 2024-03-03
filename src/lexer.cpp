#include "lexer.hpp"

#include <cctype>
#include <iostream>
#include <set>
#include <stdexcept>

std::vector<Token> lexer(const std::string &code)
{
    std::vector<Token> tokens;
    size_t i = 0;
    std::set<std::string> int_sizes = {"8", "16", "32", "64", "128"};
    std::set<std::string> float_sizes = {"32", "64"};

    while (i < code.length()) {
        // Handling numbers
        if (isdigit(code[i])) {
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
        else if (code[i] == '/') {
            tokens.push_back(Token(DIVIDE, ""));
            i++;
        }
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
        else if (code[i] == ';') {
            tokens.push_back(Token(SEMICOLON, ""));
            i++;
        }

        // Handling parentheses and braces
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

        // Handling variables and keywords
        else if (isalpha(code[i])) {
            std::string ident;
            while (i < code.length() &&
                   (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_')) {
                ident += code[i];
                i++;
            }
            // Handling keywords
            if (ident == "let") {
                tokens.push_back(Token(LET, ""));
            }
            else if (ident == "var") {
                tokens.push_back(Token(VAR, ""));
            }
            else if (ident == "if") {
                tokens.push_back(Token(IF, ""));
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
            throw std::runtime_error("Unknown character: " + std::string(1, code[i]));
        }
    }
    // Adding end-of-file character
    tokens.push_back(Token(EOF_TOKEN, ""));
    return tokens;
}

std::string tokenTypeToString(TokenType type)
{
    switch (type) {
    case LET:
        return "LET";
    case VAR:
        return "VAR";
    case NUMBER:
        return "NUMBER";
    case PLUS:
        return "PLUS";
    case TIMES:
        return "TIMES";
    case MINUS:
        return "MINUS";
    case DIVIDE:
        return "DIVIDE";
    case EQUALS:
        return "EQUALS";
    case EQUALS_EQUALS:
        return "EQUALS_EQUALS";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case LBRACE:
        return "LBRACE";
    case RBRACE:
        return "RBRACE";
    case IF:
        return "IF";
    case IDENTIFIER:
        return "IDENTIFIER";
    case EOF_TOKEN:
        return "EOF_TOKEN";
    case SEMICOLON:
        return "SEMICOLON";
    case COMMA:
        return "COMMA";
    default:
        return "UNKNOWN";
    }
}

std::string get_lexer_output(const std::vector<Token> &tokens)
{
    std::string output;
    output += "[\n";
    for (const Token &token : tokens) {
        std::string tokenTypeName = tokenTypeToString(token.first);
        output += "    ('" + tokenTypeName + "', '" + token.second + "'),\n";
        if (token.first == SEMICOLON) {
            output += "\n";
        }
    }
    output += "]";
    return output;
}

/*
        else if (code[i] == '&') {
            tokens.push_back(Token(REFERENCE, ""));
            i++;
        }
        else if (code[i] == ':') {
            tokens.push_back(Token(COLON, ""));
            i++;
        }
        else if (code[i] == ';') {
            tokens.push_back(Token(SEMICOLON", ""));
            i++;
        }

        else if (code[i] == '[') {
            tokens.push_back(Token(LSQUARE, ""));
            i++;
        }
        else if (code[i] == ']') {
            tokens.push_back(Token(RSQUARE, ""));
            i++;
        }

            else if (ident == "while") {
                tokens.push_back(Token(WHILE, ""));
            }
            else if (ident == "for") {
                tokens.push_back(Token(FOR, ""));
            }
            else if (ident == "in") {
                tokens.push_back(Token(IN, ""));
            }
            else if (ident == "struct") {
                tokens.push_back(Token(STRUCT, ""));
            }
            else if (ident == "fun") {
                tokens.push_back(Token(FUNCTION, ""));
            }

            //handling datatypes
            else if (ident == "char") {
                tokens.push_back(Token(CHARDECLAR, ""));
            }
            else if (ident[0] == 'u' && (int_sizes.find(ident.substr(1,
   ident.length()-1)) != int_sizes.end())) { tokens.push_back(Token(UDECLAR,
   ident.substr(1, ident.length()-1)));
            }
            else if (ident[0] == 'i' && (int_sizes.find(ident.substr(1,
   ident.length()-1)) != int_sizes.end())) { tokens.push_back(Token(IDECLAR,
   ident.substr(1, ident.length()-1)));
            }
            else if (ident[0] == 'f' && (float_sizes.find(ident.substr(1,
   ident.length()-1)) != float_sizes.end())) { tokens.push_back(Token(FDECLAR,
   ident.substr(1, ident.length()-1)));
            }
*/