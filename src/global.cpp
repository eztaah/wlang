#include <iostream>

#include "global.hpp"

bool debug_flag = false;
bool runtime_error_flag = false;
std::string build_directory = "build";
std::string output_location = "prog";

void display_and_trow_internal_error(std::string area, std::string error_message)
{
    std::cerr << "\n\033[1m\033[31minternal error (" << area << "): \033[0m" << error_message << "\n"
              << std::endl;

    if (runtime_error_flag) {
        throw std::runtime_error(error_message);
    }
}

void display_and_trow_base_error(std::string error_message)
{
    std::cerr << "\033[1m\033[31merror: \033[0m" << error_message << "\n"
              << std::endl;

    if (runtime_error_flag) {
        throw std::runtime_error(error_message);
    }
}

void display_and_trow_error(std::string area, int line_number, std::string error_message)
{
    std::string block1;
    std::string block2 = "";
    // handle area
    if (area == "none") {
        block1 = "error: ";
    }
    else {
        block1 = area + " error: ";
    }
    // handle line number
    if (line_number != -1) {
        block2 = "\033[1mline " + std::to_string(line_number) + ": \033[0m";
    }

    std::cerr << "\n\033[1m\033[31m" << block1 << "\033[0m" << block2 << error_message << std::endl;

    // handle build files
    if (debug_flag) {
        if (area == "parser") {
            std::cout << "  -> find lexer output in \"" << build_directory << "\"\n"
                      << std::endl;
        }
        else if (area == "generator") {
            std::cout << "  -> find lexer and parser outputs in \"" << build_directory << "\"\n"
                      << std::endl;
        }
    }
    else {
        std::cout << std::endl;
    }

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
        display_and_trow_internal_error("lexer",
                                        "in token_to_string(): \'" + std::to_string(type) + "\' does not have string equivalent");
        exit(1);
    }
}