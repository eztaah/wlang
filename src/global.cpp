#include <iostream>

#include "global.hpp"

std::string token_to_string(TokenType type)
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
    case SEMICOLON:
        return "SEMICOLON";
    default:
        std::cout << "\033[31m[!] Internal lexer error : Error in tokenTypeToString: " + std::to_string(type) + " does not have string equivalent" + "\033[0m" << std::endl;
        exit(-1);
    }
}