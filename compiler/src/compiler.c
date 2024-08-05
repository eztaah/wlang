#include <stdio.h>
#include "lib/lib.h"
#include "io.h"
#include "lexer.h"
#include "parser.h"
#include "global.h"
#include <string.h>
#include "generator.h"

static Void compile(Char* src)
{
    // Lexing
    printf("Lexing...\n");
    List* token_list = lex(src);

    // Parsing
    printf("Parsing...\n"); 
    List* node_list = parse(token_list);

    // Convert AST to string
    Char* output = strdup("");
    for (I32 i = 0; i < node_list->size; ++i) {
        StatementNode* stmt = (StatementNode*)node_list->items[i];
        output = print_ast(stmt, output, "", true, false);
    }

    // Generating assembly code
    printf("Generating assembly...\n");
    Char* asm_output = generate(node_list);

    // Write the output to a file
    write_file("out/lexer_output.txt", convert_token_list_to_char(token_list));
    write_file("out/parser_output.txt", output);
    write_file("out/asm_output.s", asm_output);

    // Clean up
    free(output);
    // TODO: free other things

}

Void compile_file(const Char* filename)
{
    Char* src = read_file(filename);
    compile(src);
    free(src);
}
