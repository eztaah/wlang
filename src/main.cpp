#include "lexer.hpp"
#include "parser.hpp"
#include "nodes.hpp"
#include <iostream>

int main() 
{
    std::string code = "a = 4 * 2 + 1; b = 2; c = a + b; if (a == b) {r = 0;}";

    // LEXER 
    std::cout << "\n---- LEXER OUTPUT ----\n" << std::endl;
    std::vector<Token> tokens = lexer(code);
    pretty_print(tokens);

    // PARSER 
    std::cout << "\n---- PARSER OUTPUT ----\n" << std::endl;
    NodePtr ast = parse(tokens);
    print_ast(ast, "", false, false);
    
    return 0;
}

