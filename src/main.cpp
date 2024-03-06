#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "asm_generator.hpp"
#include "lexer.hpp"
#include "nodes.hpp"
#include "parser.hpp"
#include "semantic_analysis.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2 || std::string(argv[1]) == "--help") {
        std::cout << "Usage:" << std::endl;
        std::cout << "    compiler <file.cp> [options]    compile a source file with optional flags." << std::endl;
        std::cout << "    compiler --version              display compiler version and exit." << std::endl;
        std::cout << "    compiler --help                 display this help message and exit." << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "    -o, --output <executable>       Specify the location and name of the output executable (default: ./prog)." << std::endl;
        std::cout << "    -b, --build-dir <directory>     Specify the build directory for intermediate files. (default: ./build/)" << std::endl;
        std::cout << "    -v, --verbose                   be verbose" << std::endl;

        std::cout << std::endl;
        return EXIT_FAILURE;
    }
    else if (std::string(argv[1]) == "--version") {
        std::cout << "compiler version 0.1" << std::endl;
        std::cout << std::endl;
        return EXIT_SUCCESS;
    }

    bool verbose = false;
    std::string build_directory = "build";
    std::string output_location = "prog";

    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-b" || std::string(argv[i]) == "--build-dir") {
            if (i + 1 < argc) {
                build_directory = argv[i + 1];
                i++;
            }
            else {
                std::cerr << "error: no build directory specified" << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if (std::string(argv[i]) == "-o" || std::string(argv[i]) == "--output") {
            if (i + 1 < argc) {
                output_location = argv[i + 1];
                i++;
            }
            else {
                std::cerr << "error: no output location specified" << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--verbose") {
            verbose = true;
        }
    }

    // handle build directory , create directory if needed
    build_directory += '/';
    std::filesystem::path build_dirPath(build_directory);
    if (!std::filesystem::exists(build_dirPath)) {
        if (!std::filesystem::create_directories(build_dirPath)) {
            std::cerr << "error: failed to create output directory" << std::endl;
            return EXIT_FAILURE;
        }
        else {
            if (verbose) {
                std::cout << "Created directory: " << build_directory << std::endl;
            }
        }
    }

    // handle output location
    output_location = "./" + output_location;
    std::filesystem::path outputPath(output_location);
    std::filesystem::path output_dirPath = outputPath.parent_path();

    if (!std::filesystem::exists(output_dirPath)) {
        if (!std::filesystem::create_directories(output_dirPath)) {
            std::cerr << "error: failed to create directory" << std::endl;
            return EXIT_FAILURE;
        } else {
            if (verbose) {
                std::cout << "Created directory: " << output_dirPath << std::endl;
            }
        }
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
    // if (verbose) {
    //     std::cout << "\n---- LEXER OUTPUT ----\n"
    //               << std::endl;
    //     std::cout << lexer_output << std::endl;
    // }
    // Write result to file
    std::ofstream output1(build_directory + "1_lexer_output.txt");
    output1 << lexer_output;
    output1.close();

    // PARSER
    NodePtr ast = parse(tokens);
    std::string parser_output1;
    std::string parser_output2 = print_ast(ast, parser_output1, "", false, false);

    // PARSER OUTPUT
    // if (verbose) {
    //     std::cout << "\n\n---- PARSER OUTPUT ----\n"
    //               << std::endl;
    //     std::cout << parser_output2 << std::endl;
    // }
    // Write result to file
    std::ofstream output2(build_directory + "2_parser_output.txt");
    output2 << parser_output2;
    output2.close();

    // SEMANTIC ANALYSIS
    // analyzeAST(ast);

    // GENERATOR
    std::string asm_final_output = "";

    asm_final_output = generate_assembly(ast);
    if (true) { // TODO : need to check if directory exists
        std::ofstream output(build_directory + "3_generator_output.asm");
        output << asm_final_output;
        output.close();
    }

    std::string cpp_final_output = "";

    // Print if verbose
    // if (verbose) {
    //     // ASSEMBLY
    //     std::cout << "\n\n---- ASM_GENERATOR OUTPUT ----\n"
    //               << std::endl;
    //     std::cout << asm_final_output << std::endl;
    //     std::cout << "------------------------------\n"
    //               << std::endl;
    // }

    // assembling with nasm
    std::string nasm_command = "nasm -f elf64 " + build_directory + "3_generator_output.asm -o " + build_directory + "4_nasm_output.o";
    system(nasm_command.c_str());

    // liking with gnu ld
    std::string ld_command = "ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o" + output_location + " " + build_directory + "4_nasm_output.o -lc";
    system(ld_command.c_str());

    std::cout << "Build written to \"" << build_directory << "\"" << std::endl;
    std::cout << "Output written to \"" << output_location << "\"" << std::endl;

    return 0;
}
