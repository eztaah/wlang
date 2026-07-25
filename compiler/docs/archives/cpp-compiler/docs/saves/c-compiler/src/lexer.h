#ifndef LEXER_H
#define LEXER_H

#define LEXER_OUTPUT_SIZE 100

typedef enum {
    NUMBER, 
    PLUS, 
    TIMES, 
    EQUALS, 
    EQUALS_EQUALS, 
    SEMICOLON,
    LPAREN,         // Left parenthesis
    RPAREN,         // Right parenthesis
    LBRACE,         // Left brace
    RBRACE,         // Right brace
    IF, 
    IDENTIFIER, 
    EOF_TOKEN       // End of file token
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;


// Functions
void addToken(Token **tokens, int *tokenCount, TokenType type, char *value);

Token* lexer(char *code, int *tokenCount);

void pretty_print(Token *tokens, const int *tokenCount);

#endif // LEXER_H
