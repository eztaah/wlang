#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "parser.h"

int main() {
    char code[] = "a = 4 * 2 + 1; b = 2; if (a == b) {r = 0;}";

    // LEXER
    int tokenCount = 0;
    Token *tokens = lexer(code, &tokenCount);
    pretty_print(tokens, &tokenCount);

    // PARSER
    Node* programNode = parse_prog(tokens, tokenCount);
    printf("\n---- PARSER OUTPUT ----\n");
    print_ast(programNode, "", 0, 1);

    return 0;
}
