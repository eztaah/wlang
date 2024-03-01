// lexer.cpp
#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>


std::vector<Token> lexer(const std::string& code) 
{
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < code.length()) {

        // Handling numbers
        if (isdigit(code[i])) {
            std::string num;
            while (i < code.length() && isdigit(code[i])) {
                num += code[i];
                i++;
            }
            tokens.push_back(Token("NUMBER", num));
        }

        // Handling operators
        else if (code[i] == '+') {
            tokens.push_back(Token("PLUS", ""));
            i++;
        }
        else if (code[i] == '*') {
            tokens.push_back(Token("TIMES", ""));
            i++;
        }
        else if (code[i] == '=') {
            if (i + 1 < code.length() && code[i + 1] == '=') {
                tokens.push_back(Token("EQUALS_EQUALS", ""));
                i += 2;
            }
            else {
                tokens.push_back(Token("EQUALS", ""));
                i++;
            }
        }
        else if (code[i] == ';') {
            tokens.push_back(Token("SEMICOLON", ""));
            i++;
        }

        // Handling parentheses and braces
        else if (code[i] == '(') {
            tokens.push_back(Token("LPAREN", ""));
            i++;
        }
        else if (code[i] == ')') {
            tokens.push_back(Token("RPAREN", ""));
            i++;
        }
        else if (code[i] == '{') {
            tokens.push_back(Token("LBRACE", ""));
            i++;
        }
        else if (code[i] == '}') {
            tokens.push_back(Token("RBRACE", ""));
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
            if (ident == "if") {
                tokens.push_back(Token("IF", ""));
            }
            else {
                tokens.push_back(Token("IDENTIFIER", ident));
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
    tokens.push_back(Token("EOF", ""));
    return tokens;
}




void pretty_print(const std::vector<Token>& tokens)
{
    std::cout << "[" << std::endl;
    for (const Token& token : tokens) {
        std::cout << "    ('" << token.first << "', " << token.second << ")," << std::endl;
    }
    std::cout << "]" << std::endl;
}