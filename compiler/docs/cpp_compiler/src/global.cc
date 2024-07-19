#include <iostream>

#include "global.hh"


bool dev_mode = false;
bool to_executable = false;
bool runtime_error_flag = false;
std::string output_directory = "out";
int var_decl_count = 0;

void display_and_throw_internal_error(std::string error_message)
{
    std::cerr << "\n\033[1m\033[31minternal error: \033[0m" << error_message << "\n"
              << std::endl;

    if (runtime_error_flag) {
        throw std::runtime_error(error_message);
    }
}

void display_and_throw_error(std::string error_message, int line_number)
{
    std::string block2 = "";

    // handle line number
    if (line_number != -1) {
        block2 = "line " + std::to_string(line_number) + ":";
    }

    std::cerr << "\n\033[1m" << block2 << "\033[31m error: "
              << "\033[0m" << error_message << "\n"
              << std::endl;

    // runtimes errors
    if (runtime_error_flag) {
        throw std::runtime_error(error_message);
    }
}

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
        display_and_throw_internal_error("in token_to_string(): \'" + std::to_string(type) + "\' does not have string equivalent");
        exit(1);
    }
}