#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "asm_generator.hh"
#include "lexer.hh"
#include "parser.hh"


void print_usage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    compiler <file.cp> [options]    compile a source file with optional flags." << std::endl;
    std::cout << "    compiler --version              display compiler version and exit." << std::endl;
    std::cout << "    compiler --help                 display this help message and exit." << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "    -o, --output-dir <directory>    specify the output directory. (default: ./out/)" << std::endl;
    std::cout << "    -d, --dev-mode                  activate dev mode" << std::endl;
    std::cout << "    -r, --runtime-error             throw runtime errors when an error occurs (useful for debugging)" << std::endl;
    std::cout << std::endl;
    std::cout << "    -e, --to-executable             assemble and link the generated assembly code into an executable" << std::endl;
    std::cout << "                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time." << std::endl;
    std::cout << "                                        - the generated executable will only be able to run on x86_64 CPUs and in a Linux environment." << std::endl;
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
        std::cout << "compiler version 1.0" << std::endl;
        std::cout << std::endl;
        return EXIT_SUCCESS;
    }

    // handle optionnal arguements
    for (int i = 2; i < argc; i++) {

        if (std::string(argv[i]) == "-e" || std::string(argv[i]) == "--to-executable ") {
            to_executable = true;
        }

        else if (std::string(argv[i]) == "-o" || std::string(argv[i]) == "--output-dir") {
            if (i + 1 < argc) {
                output_directory = argv[i + 1];
                i++;
            }
            else {
                display_and_throw_error("-o or --output-dir argument used, but no directory specified");
                return EXIT_FAILURE;
            }
        }

        else if (std::string(argv[i]) == "-d" || std::string(argv[i]) == "--dev-mode") {
            dev_mode = true;
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
    output_directory = "./" + output_directory;
    output_directory += '/';
    std::filesystem::path build_dirPath(output_directory);
    if (!std::filesystem::exists(build_dirPath)) {
        if (!std::filesystem::create_directories(build_dirPath)) {
            display_and_throw_error("failed to create build directory");
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
    lexer_output += get_printable_lexer_output(tokens);
    // write result to file
    std::ofstream output1(output_directory + "1_lexer_output.txt");
    output1 << lexer_output;
    output1.close();
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // PARSER
    std::cout << "\033[96m2. parsing... \033[0m" << std::flush;
    NodePtr ast = parse(tokens);
    std::string parser_output1;
    std::string parser_output2 = print_ast(ast, parser_output1, "", false, false);
    // write result to file
    std::ofstream output2(output_directory + "2_parser_output.txt");
    output2 << parser_output2;
    output2.close();
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // GENERATOR
    std::cout << "\033[96m3. generating assembly code... \033[0m" << std::flush;
    std::string asm_final_output = "";
    asm_final_output = generate_assembly(ast);
    // write result to file
    std::ofstream output(output_directory + "3_generator_output.s");
    output << asm_final_output;
    output.close();
    std::cout << "\033[96mdone\033[0m" << std::endl;

    // ASSEMBLING AND LINKING
    if (to_executable) {
        // assembling with gas ( as ./build/3_generator_output.s -o ./build/4_gas_output.o )
        std::cout << "\033[96m4. assembling with gas... \033[0m" << std::flush;
        std::string gas_command = "as -o " + output_directory + "4_gas_output.o " + output_directory + "3_generator_output.s";
        if (system(gas_command.c_str()) != 0) {
            display_and_throw_error("gas error");
            return EXIT_FAILURE;
        }
        std::cout << "\033[96mdone\033[0m" << std::endl;

        // liking with gnu ld ( ld ./build/4_gas_output.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o ./build/prog )
        std::cout << "\033[96m5. linking with gnu ld... \033[0m" << std::flush;
        std::string ld_command = "ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o" + output_directory + "prog " + output_directory + "4_gas_output.o -lc";
        if (system(ld_command.c_str()) != 0) {
            display_and_throw_error("GNU ld error");
            return EXIT_FAILURE;
        }
        std::cout << "\033[96mdone\033[0m" << std::endl;
    }

    std::cout << "\n-> output location : \"" << output_directory << "\"" << std::endl;
    return 0;
}
