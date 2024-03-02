#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer.hpp"
#include "parser.hpp"
#include "nodes.hpp"
#include "asm_generator.hpp"
#include "cpp_generator.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "usage: compiler <file.cp> [--cpp] [--asm] [--output output_file]" << std::endl;
        return EXIT_FAILURE;
    }

    bool isModeAsm = false;
    bool isModeCpp = false;

    std::string outputFile;

    for (int i = 2; i < argc; i++)
    {
        if (std::string(argv[i]) == "--asm")
        {
            isModeAsm = true;
        }
        else if (std::string(argv[i]) == "--cpp")
        {
            isModeCpp = true;
        }
        else if (std::string(argv[i]) == "-o" || std::string(argv[i]) == "--output")
        {
            if (i + 1 < argc)
            {
                outputFile = argv[i + 1];
                i++;
            }
            else
            {
                std::cerr << "error: no output file specified" << std::endl;
                return EXIT_FAILURE;
            }
        }
    }

    if (isModeAsm && isModeCpp)
    {
        std::cerr << "error: cannot use both --asm and --cpp flags" << std::endl;
        return EXIT_FAILURE;
    }

    if (!isModeAsm && !isModeCpp)
    {
        std::cerr << "error: no mode specified, use --asm or --cpp" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filePath = argv[1];
    std::ifstream fileStream(filePath);
    std::string code;

    if (fileStream)
    {
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        code = buffer.str();
    }
    else
    {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return 1; // Return avec un code d'erreur
    }

    // LEXER
    std::cout << "\n---- LEXER OUTPUT ----\n"
              << std::endl;
    std::vector<Token> tokens = lexer(code);
    print_lexer_output(tokens);

    // PARSER
    std::cout << "\n\n---- PARSER OUTPUT ----\n"
              << std::endl;
    NodePtr ast = parse(tokens);
    print_ast(ast, "", false, false);

    std::string final_output = "";

    if (isModeAsm)
    {

        // ASSEMBLY
        std::cout << "\n\n---- ASM_GENERATOR OUTPUT ----\n"
                  << std::endl;
        std::unordered_map<std::string, std::string> variables;
        auto assembly_code_pair = generate_assembly(ast, variables);
        auto assembly_code = assembly_code_pair.first;

        std::string asm_final_output = "";

        asm_final_output += "global _start\n\n";
        asm_final_output += "_start:\n";
        for (const auto &instruction : assembly_code) 
        {
            asm_final_output += instruction + '\n';
        }
        asm_final_output += "; exit syscall\n";
        asm_final_output += "mov rax, 60\n";
        asm_final_output += "mov rdi, 0\n";
        asm_final_output += "syscall\n";
        
        std::cout << asm_final_output << std::endl;
        std::cout << "----------------------" << std::endl;

        final_output = asm_final_output;
    }

    if (isModeCpp)
    {
        std::cout << "\n\n---- CPP_GENERATOR OUTPUT ----\n"
                  << std::endl;
        final_output = generate_cpp(ast);
        std::cout << final_output << std::endl;
        std::cout << "----------------------" << std::endl;
    }

    if (!outputFile.empty())
    {
        std::ofstream output(outputFile);
        output << final_output;
        output.close();
        std::cout << "Output written to " << outputFile << std::endl;
    }

    return 0;
}
