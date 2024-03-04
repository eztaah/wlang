#include <fstream>
#include <iostream>
#include <sstream>

#include "asm_generator.hpp"
#include "cpp_generator.hpp"
#include "lexer.hpp"
#include "nodes.hpp"
#include "parser.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "usage: compiler <file.cp> [--cpp] [--asm] [--output output_file]" << std::endl;
        return EXIT_FAILURE;
    }

    bool verbose = false;
    bool isModeAsm = false;
    bool isModeCpp = false;
    std::string outputDir = "build/";

    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "--asm") {
            isModeAsm = true;
        }
        else if (std::string(argv[i]) == "--cpp") {
            isModeCpp = true;
        }
        else if (std::string(argv[i]) == "-o" ||
                 std::string(argv[i]) == "--output") {
            if (i + 1 < argc) {
                outputDir = argv[i + 1];
                i++;
            }
            else {
                std::cerr << "error: no output directory specified" << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if (std::string(argv[i]) == "-v" ||
                 std::string(argv[i]) == "--verbose") {
            verbose = true;
        }
    }

    // if (isModeAsm && isModeCpp)
    // {
    //     std::cerr << "error: cannot use both --asm and --cpp flags" <<
    //     std::endl; return EXIT_FAILURE;
    // }

    if (!isModeAsm && !isModeCpp) {
        isModeAsm = true;
        isModeCpp = true;
        // std::cerr << "error: no mode specified, use --asm or --cpp" <<
        // std::endl; return EXIT_FAILURE;
    }

    std::string filePath = argv[1];
    std::ifstream fileStream(filePath);
    std::string code;

    if (fileStream) {
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        code = buffer.str();
    }
    else {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return 1; // Return avec un code d'erreur
    }

    // LEXER
    std::vector<Token> tokens = lexer(code);
    std::string lexer_output = get_lexer_output(tokens);
    if (verbose) {
        std::cout << "\n---- LEXER OUTPUT ----\n"
                  << std::endl;
        std::cout << lexer_output << std::endl;
    }
    // Write result to file
    std::ofstream output1(outputDir + "lexer.out");
    output1 << lexer_output;
    output1.close();

    // PARSER
    NodePtr ast = parse(tokens);
    std::string parser_output1;
    std::string parser_output2 = print_ast(ast, parser_output1, "", false, false);

    // PARSER OUTPUT
    if (verbose) {
        std::cout << "\n\n---- PARSER OUTPUT ----\n"
                  << std::endl;
        std::cout << parser_output2 << std::endl;
    }
    // Write result to file
    std::ofstream output2(outputDir + "parser.out");
    output2 << parser_output2;
    output2.close();

    std::string asm_final_output = "";

    if (isModeAsm) {
        // ASSEMBLY
        asm_final_output = generate_assembly(ast);

        if (true) { // TODO : need to check if directory exists
            std::ofstream output(outputDir + "output.asm");
            output << asm_final_output;
            output.close();
        }
    }

    std::string cpp_final_output = "";

    if (isModeCpp) {
        cpp_final_output = generate_cpp(ast);

        if (true) { // TODO : need to check if directory exists
            std::ofstream output(outputDir + "output.cpp");
            output << cpp_final_output;
            output.close();
        }
    }

    // Print if verbose
    if (verbose) {
        if (isModeAsm) {
            // ASSEMBLY
            std::cout << "\n\n---- ASM_GENERATOR OUTPUT ----\n"
                      << std::endl;
            std::cout << asm_final_output << std::endl;
            std::cout << "------------------------------\n"
                      << std::endl;
        }
        if (isModeCpp) {
            // CPP GENERATOR OUTPUT
            std::cout << "\n\n---- CPP_GENERATOR OUTPUT ----\n"
                      << std::endl;
            std::cout << cpp_final_output << std::endl;
            std::cout << "------------------------------\n"
                      << std::endl;
        }
    }

    std::cout << "Output written to \"" << outputDir << "\"" << std::endl;

    return 0;
}
