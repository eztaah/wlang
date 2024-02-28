#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

int main() {
    char code[] = "a = 4 * 2 + 1; b = 2; if (a == b) {r = 0;}";

    // LEXER
    int tokenCount = 0;
    Token *tokens = lexer(code, &tokenCount);
    pretty_print(tokens, &tokenCount);

    return 0;
}
