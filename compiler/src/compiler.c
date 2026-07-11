#include <stdio.h>
#include "lib/lib.h"
#include "io.h"
#include "lexer.h"


static Void compile(Char* src)
{
    // Lexing
    printf("Lexing...\n");
    List* token_list = lex(src);

    // Write the output in a file
    write_file("out/lexer_output.txt", convert_token_list_to_char(token_list));
}

Void compile_file(const Char* filename)
{
    Char* src = read_file(filename);
    compile(src);
    free(src);
}