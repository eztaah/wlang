#ifndef GLOBAL_H
#define GLOBAL_H

/* Create new types  */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int int16_t;
typedef unsigned short int uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char bool123;

/* Useful defines */
#define TRUE 1
#define FALSE 0

/* enum */
typedef enum {
    CST,
    VAR,
    NUMBER,
    IDENTIFIER,
    EQUALS,

    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    MODULO,
    BIN_AND,
    BIN_OR,
    XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,

    WHILE,
    IF,
    ELSE,

    END_LINE,
    COMMA,
    QUOTE,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COLON,
    TYPE,

    EQUALS_EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    LESS_THAN_EQUALS,
    GREATER_THAN,
    GREATER_THAN_EQUALS,
    AND,
    OR,

    UNDEFINED,
    EOF_TOKEN
} token_type;

typedef struct Token {
    int32_t line_number;
    token_type type;
    const char *value;
} Token;

#endif // GLOBAL_H
