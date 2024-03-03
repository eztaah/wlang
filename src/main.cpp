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
    std::cerr
        << "usage: compiler <file.cp> [--cpp] [--asm] [--output output_file]"
        << std::endl;
    return EXIT_FAILURE;
  }

  bool isModeAsm = false;
  bool isModeCpp = false;

  std::string outputDir;

  for (int i = 2; i < argc; i++) {
    if (std::string(argv[i]) == "--asm") {
      isModeAsm = true;
    } else if (std::string(argv[i]) == "--cpp") {
      isModeCpp = true;
    } else if (std::string(argv[i]) == "-o" ||
               std::string(argv[i]) == "--output") {
      if (i + 1 < argc) {
        outputDir = argv[i + 1];
        i++;
      } else {
        std::cerr << "error: no output directory specified" << std::endl;
        return EXIT_FAILURE;
      }
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
    // std::cerr << "error: no mode specified, use --asm or --cpp" << std::endl;
    // return EXIT_FAILURE;
  }

  std::string filePath = argv[1];
  std::ifstream fileStream(filePath);
  std::string code;

  if (fileStream) {
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    code = buffer.str();
  } else {
    std::cerr << "Unable to open file: " << filePath << std::endl;
    return 1; // Return avec un code d'erreur
  }

  // LEXER
  std::cout << "\n---- LEXER OUTPUT ----\n" << std::endl;
  std::vector<Token> tokens = lexer(code);
  std::string lexer_output = get_lexer_output(tokens);
  std::cout << lexer_output << std::endl;
  // Write result to file
  std::ofstream output1(outputDir + "lexer.out");
  output1 << lexer_output;
  output1.close();

  // PARSER
  std::cout << "\n\n---- PARSER OUTPUT ----\n" << std::endl;
  NodePtr ast = parse(tokens);
  std::string parser_output1;
  std::string parser_output2 = print_ast(ast, parser_output1, "", false, false);
  std::cout << parser_output2 << std::endl;
  // Write result to file
  std::ofstream output2(outputDir + "parser.out");
  output2 << parser_output2;
  output2.close();

  std::string final_output = "";

  if (isModeAsm) {

    // ASSEMBLY
    std::cout << "\n\n---- ASM_GENERATOR OUTPUT ----\n" << std::endl;
    std::unordered_map<std::string, std::string> variables;
    auto assembly_code_pair = generate_assembly(ast, variables);
    auto assembly_code = assembly_code_pair.first;

    std::string asm_final_output = "";

    asm_final_output += "global _start\n\n";
    asm_final_output += "_start:\n";
    for (const auto &instruction : assembly_code) {
      asm_final_output += instruction + '\n';
    }
    asm_final_output += "; exit syscall\n";
    asm_final_output += "mov rax, 60\n";
    asm_final_output += "mov rdi, 0\n";
    asm_final_output += "syscall\n";

    std::cout << asm_final_output << std::endl;
    std::cout << "----------------------" << std::endl;

    final_output = asm_final_output;

    if (true) // TODO : need to check if directory exists
    {
      std::ofstream output(outputDir + "output.asm");
      output << final_output;
      output.close();
      std::cout << "Output written to " << outputDir << std::endl;
    }
  }

  if (isModeCpp) {
    std::cout << "\n\n---- CPP_GENERATOR OUTPUT ----\n" << std::endl;
    final_output = generate_cpp(ast);
    std::cout << final_output << std::endl;
    std::cout << "----------------------" << std::endl;

    if (true) // TODO : need to check if directory exists
    {
      std::ofstream output(outputDir + "output.cpp");
      output << final_output;
      output.close();
      std::cout << "Output written to " << outputDir << std::endl;
    }
  }

  return 0;
}
