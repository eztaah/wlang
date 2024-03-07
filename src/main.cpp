#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "asm_generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semantic_analysis.hpp"

void print_usage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    compiler <file.cp> [options]    compile a source file with optional flags." << std::endl;
    std::cout << "    compiler --version              display compiler version and exit." << std::endl;
    std::cout << "    compiler --help                 display this help message and exit." << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "    -o, --output <executable>       Specify the location and name of the output executable (default: ./prog)." << std::endl;
    std::cout << "    -b, --build-dir <directory>     Specify the build directory for intermediate files. (default: ./build/)" << std::endl;
    std::cout << "    -d, --dev-mode                  activate dev mode" << std::endl;
    std::cout << "    -r, --runtime-error             throw runtime errors when an error occur" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    // handle base arguments
    if (argc < 2) {
        print_usage();
        return EXIT_FAILURE;
    }
    else if (std::string(argv[1]) == "--help") {
        print_usage();
        return EXIT_SUCCESS;
    }
    else if (std::string(argv[1]) == "--version") {
        std::cout << "compiler version 0.1" << std::endl;
        std::cout << std::endl;
        return EXIT_SUCCESS;
    }

    // handle optionnal arguements
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-b" || std::string(argv[i]) == "--build-dir") {
            if (i + 1 < argc) {
                build_directory = argv[i + 1];
                i++;
            }
            else {
                display_and_throw_error("-b or --build-dir argument used, but no directory specified");
                return EXIT_FAILURE;
            }
        }
        else if (std::string(argv[i]) == "-o" || std::string(argv[i]) == "--output") {
            if (i + 1 < argc) {
                output_location = argv[i + 1];
                i++;
            }
            else {
                display_and_throw_error("-o or --output argument used, but no output specified");
                return EXIT_FAILURE;
            }
        }
        else if (std::string(argv[i]) == "-d" || std::string(argv[i]) == "--dev-mode") {
            debug_flag = true;
        }
        else if (std::string(argv[i]) == "-r" || std::string(argv[i]) == "--runtime-error") {
            runtime_error_flag = true;
        }
        // if a wrong argument is given
        else {
            display_and_throw_error(std::string(argv[i]) + " is not a valid argument");
            print_usage();
            return EXIT_FAILURE;
        }
    }

    // handle build directory , create directory if needed
    build_directory = "./" + build_directory;
    build_directory += '/';
    std::filesystem::path build_dirPath(build_directory);
    if (!std::filesystem::exists(build_dirPath)) {
        if (!std::filesystem::create_directories(build_dirPath)) {
            display_and_throw_error("failed to create build directory");
            return (EXIT_FAILURE);
        }
    }

    // handle output location
    output_location = "./" + output_location;
    std::filesystem::path outputPath(output_location);
    std::filesystem::path output_dirPath = outputPath.parent_path();
    if (!std::filesystem::exists(output_dirPath)) {
        if (!std::filesystem::create_directories(output_dirPath)) {
            display_and_throw_error("failed to create output directory");
            return (EXIT_FAILURE);
        }
    }

    // open the input file
    std::string filePath = argv[1];
    std::ifstream fileStream(filePath);
    std::string code;
    if (fileStream) {
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        code = buffer.str();
    }
    else {
        display_and_throw_error("unable to open file : " + filePath);
        return (EXIT_FAILURE);
    }

    // LEXER
    std::cout << "\033[96m1. lexing... \033[0m" << std::flush;
    std::vector<Token> tokens = lex(code);
    std::string lexer_output;
    lexer_output += "LEXER OUTPUT\n";
    lexer_output += "=> output type : std::vector<std::pair<TokenType, std::string>>\n\n";
    lexer_output += get_printable_lexer_output(tokens);
    // Write result to file
    std::ofstream output1(build_directory + "1_lexer_output.txt");
    output1 << lexer_output;
    output1.close();
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // PARSER
    std::cout << "\033[96m2. parsing... \033[0m" << std::flush;
    NodePtr ast = parse(tokens);
    std::string parser_output1;
    std::string parser_output2 = print_ast(ast, parser_output1, "", false, false);
    // Write result to file
    std::ofstream output2(build_directory + "2_parser_output.txt");
    output2 << parser_output2;
    output2.close();
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // SEMANTIC ANALYSIS
    // analyzeAST(ast);

    // GENERATOR
    std::cout << "\033[96m3. generating assembly code... \033[0m" << std::flush;
    std::string asm_final_output = "";
    asm_final_output = generate_assembly(ast);
    // Write result to file
    std::ofstream output(build_directory + "3_generator_output.asm");
    output << asm_final_output;
    output.close();
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // ASSEMBLING
    // assembling with nasm
    std::cout << "\033[96m4. assembling with nasm... \033[0m" << std::flush;
    std::string nasm_command = "nasm -f elf64 " + build_directory + "3_generator_output.asm -o " + build_directory + "4_nasm_output.o";
    system(nasm_command.c_str());
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // liking with gnu ld
    std::cout << "\033[96m5. linking with gnu ld... \033[0m" << std::flush;
    std::string ld_command = "ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o" + output_location + " " + build_directory + "4_nasm_output.o -lc";
    system(ld_command.c_str());
    std::cout << "\033[96mdone\033[0m" << std::endl;

    std::cout << "\n-> compilation artifacts location : \"" << build_directory << "\"" << std::endl;
    std::cout << "-> executable location : \"" << output_location << "\"\n"
              << std::endl;

    return 0;
}
