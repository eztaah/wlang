#include "lexer.hpp"
#include "iostream"

int main() 
{
    std::string code = "a = 4 * 2 + 1; b = 2; if (a == b) {r = 0;}";

    // LEXER 
    std::cout << "\n---- LEXER OUTPUT ----\n" << std::endl;
    std::vector<Token> tokens = lexer(code);
    pretty_print(tokens);

    return 0;
}

