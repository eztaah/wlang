#include "lexer.hpp"
#include "parser.hpp"
#include "asm_generator.hpp"
#include "nodes.hpp"
#include <iostream>

int main() 
{
    std::string code = "a = 4 + 2 * 7; b = 2; c = a + b; if (a == b) {r = 0;}";

    // LEXER 
    std::cout << "\n---- LEXER OUTPUT ----\n" << std::endl;
    std::vector<Token> tokens = lexer(code);
    pretty_print(tokens);

    // PARSER 
    std::cout << "\n\n---- PARSER OUTPUT ----\n" << std::endl;
    NodePtr ast = parse(tokens);
    print_ast(ast, "", false, false);

    // ASSEMBLY
    std::cout << "\n\n---- ASM_GENERATOR OUTPUT ----\n" << std::endl;
    std::unordered_map<std::string, std::string> variables;
    auto assembly_code_pair = generate_assembly(ast, variables);
    auto assembly_code = assembly_code_pair.first;
    std::cout << "global _start\n" << std::endl;
    std::cout << "_start:" << std::endl;
    for (const auto& instruction : assembly_code) {
        std::cout << instruction << std::endl;
    }
    std::cout << "; exit syscall" << std::endl;
    std::cout << "mov rax, 60" << std::endl;
    std::cout << "mov rdi, 0" << std::endl;
    std::cout << "syscall" << std::endl;
    std::cout << "----------------------" << std::endl;

    return 0;
}

