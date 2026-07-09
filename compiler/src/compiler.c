#include <stdio.h>
#include "lib/lib.h"
#include "io.h"
#include "lexer.h"


static Void compile(Char* src)
{
    // Lexing
    printf("Lexing...\n");
    List* lexer_output = lex(src);

    printf("coucou\n");

    // Write the output in a file
    write_file("out/lexer_output.txt", convert_lexer_output_to_char(lexer_output));
}

Void compile_file(const Char* filename)
{
    Char* src = read_file(filename);
    compile(src);
    free(src);
}