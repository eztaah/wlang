#include <cctype>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>

#include "lexer.hh"


std::string binaryToDecimal(const std::string &binary)
{
    int decimal = 0;
    int length = binary.length();
    for (int i = 0; i < length; ++i) {
        if (binary[i] == '1') {
            decimal += std::pow(2, length - i - 1);
        }
    }

    return std::to_string(decimal);
}

std::vector<Token> lex(const std::string &code)
{
    std::vector<Token> tokens;
    size_t i = 0;
    int line_number = 1;

    while (i < code.length()) {

        // ignoring comments
        if (code[i] == '#') {
            while (i < code.length() && code[i] != '\n') {
                i++;
            }
        }

        // ignoring multi-lines comments
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
            if (!tokens.empty() && tokens.back().type != LBRACE && tokens.back().type != END_LINE && tokens.back().type != RBRACE) {
                tokens.push_back(Token(line_number, END_LINE, ""));
            }
            // Skip extra line breaks
            while (i + 1 < code.length() && code[i + 1] == '\n') {
                i++;
                line_number++;
            }
            line_number++;
            i++;
        }

        // handling binary and hexadecimal numbers
        else if (code[i] == '0' && i + 1 < code.length() && code[i + 1] == 'b') {
            std::string num = "";
            i += 2;
            while (i < code.length() && isxdigit(code[i])) {
                num += code[i];
                i++;
            }
            num = binaryToDecimal(num);
            tokens.push_back(Token(line_number, NUMBER, num));
        }

        // handling numbers and minus operator
        else if (code[i] == '-') {
            if (i + 1 < code.length() && isdigit(code[i + 1])) {
                std::string num = "-";
                i++;
                while (i < code.length() && isdigit(code[i])) {
                    num += code[i];
                    i++;
                }
                tokens.push_back(Token(line_number, NUMBER, num));
            }
            else {
                tokens.push_back(Token(line_number, MINUS, ""));
                i++;
            }
        }
        else if (isdigit(code[i])) {
            std::string num;
            while (i < code.length() && isdigit(code[i])) {
                num += code[i];
                i++;
            }
            tokens.push_back(Token(line_number, NUMBER, num));
        }

        // handling operators
        else if (code[i] == '+') {
            tokens.push_back(Token(line_number, PLUS, ""));
            i++;
        }
        else if (code[i] == '*') {
            tokens.push_back(Token(line_number, TIMES, ""));
            i++;
        }
        else if (code[i] == '/' && code[i + 1] == '/') {
            tokens.push_back(Token(line_number, DIVIDE, ""));
            i += 2;
        }
        else if (code[i] == '%') {
            tokens.push_back(Token(line_number, MODULO, ""));
            i++;
        }
        else if (code[i] == '&') {
            if (i + 1 < code.length() && code[i + 1] == '&') {
                tokens.push_back(Token(line_number, AND, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(line_number, BIN_AND, ""));
                i++;
            }
        }
        else if (code[i] == '|') {
            if (i + 1 < code.length() && code[i + 1] == '|') {
                tokens.push_back(Token(line_number, OR, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(line_number, BIN_OR, ""));
                i++;
            }
        }
        else if (code[i] == '^') {
            tokens.push_back(Token(line_number, XOR, ""));
            i++;
        }

        // handling =, ==, >, <=, etc
        else if (code[i] == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(line_number, EQUALS_EQUALS, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(line_number, EQUALS, ""));
                i++;
            }
        }
        else if (code[i] == '!' && code[i + 1] == '=') {
            tokens.push_back(Token(line_number, NOT_EQUALS, ""));
            i += 2;
        }
        else if (code[i] == '<') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(line_number, LESS_THAN_EQUALS, ""));
                i += 2;
            }
            else if (i + 1 < code.length() && code[i + 1] == '<') {
                tokens.push_back(Token(line_number, SHIFT_LEFT, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(line_number, LESS_THAN, ""));
                i++;
            }
        }
        else if (code[i] == '>') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token(line_number, GREATER_THAN_EQUALS, ""));
                i += 2;
            }
            else if (i + 1 < code.length() && code[i + 1] == '>') {
                tokens.push_back(Token(line_number, SHIFT_RIGHT, ""));
                i += 2;
            }
            else {
                tokens.push_back(Token(line_number, GREATER_THAN, ""));
                i++;
            }
        }

        // handling symbols
        else if (code[i] == '(') {
            tokens.push_back(Token(line_number, LPAREN, ""));
            i++;
        }
        else if (code[i] == ')') {
            tokens.push_back(Token(line_number, RPAREN, ""));
            i++;
        }
        else if (code[i] == '{') {
            tokens.push_back(Token(line_number, LBRACE, ""));
            i++;
        }
        else if (code[i] == '}') {
            tokens.push_back(Token(line_number, RBRACE, ""));
            i++;
        }
        else if (code[i] == ',') {
            tokens.push_back(Token(line_number, COMMA, ""));
            i++;
        }
        else if (code[i] == ':') {
            tokens.push_back(Token(line_number, COLON, ""));
            i++;
        }

        // handling strings
        else if (code[i] == '"') {
            tokens.push_back(Token(line_number, QUOTE, ""));
            i++;
            std::string res = "";
            while (code[i] != '"') {
                res += code[i];
                i++;
            }
            tokens.push_back(Token(line_number, IDENTIFIER, res));
            tokens.push_back(Token(line_number, QUOTE, ""));
            i++;
        }

        // handling variables and keywords
        else if (isalpha(code[i])) {
            std::string ident;
            while (i < code.length() && (isalpha(code[i]) || isdigit(code[i]) || code[i] == '_')) {
                ident += code[i];
                i++;
            }
            // handling keywords
            if (ident == "cst") {
                tokens.push_back(Token(line_number, CST, ""));
            }
            else if (ident == "var") {
                tokens.push_back(Token(line_number, VAR, ""));
            }
            else if (ident == "if") {
                tokens.push_back(Token(line_number, IF, ""));
            }
            else if (ident == "while") {
                tokens.push_back(Token(line_number, WHILE, ""));
            }
            else if (ident == "else") {
                tokens.push_back(Token(line_number, ELSE, ""));
            }
            else if (ident == "i64") {
                tokens.push_back(Token(line_number, TYPE, "i64"));
            }

            // handling var names
            else {
                tokens.push_back(Token(line_number, IDENTIFIER, ident));
            }
        }

        // ignoring spaces, tabs, new lines
        else if (code[i] == ' ' || code[i] == '\t') {
            i++;
        }

        // error otherwise
        else {
            display_and_throw_error("character '" + std::string(1, code[i]) + "' (ASCII: " + std::to_string(static_cast<int>(code[i])) + ") is not valid", line_number);
            exit(1);
        }
    }
    // adding end-of-file character
    tokens.push_back(Token(line_number, EOF_TOKEN, ""));
    return tokens;
}

std::string get_printable_lexer_output(const std::vector<Token> &tokens)
{
    std::string output;
    int ident_level = 0;
    output += "[\n";

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token &token = tokens[i];
        std::string token_type_name = token_to_string(token.type);
        std::string indent = std::string(ident_level * 4, ' '); // 4 spaces per indentation

        if (token.type == END_LINE) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == RBRACE) {
                output += indent + "    (" + std::to_string(token.line_number) + ", " + token_type_name + ", '" + token.value + "'),\n";
            }
            else {
                output += indent + "    (" + std::to_string(token.line_number) + ", " + token_type_name + ", '" + token.value + "'),\n\n";
            }
        }
        else if (token.type == RBRACE) {
            ident_level--;
            indent = std::string(ident_level * 4, ' ');

            output += indent + "    (" + std::to_string(token.line_number) + ", " + token_type_name + ", '" + token.value + "'),\n\n";
        }
        else {
            // classic tokens
            output += indent + "    (" + std::to_string(token.line_number) + ", " + token_type_name + ", '" + token.value + "'),\n";
        }

        if (token.type == LBRACE) {
            ident_level++;
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