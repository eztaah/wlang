#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"


typedef struct NumberNode {
    char *value;
} NumberNode;

typedef struct VarRefNode {
    char *name;
} VarRefNode;

typedef struct BinOpNode {
    struct Node* left;
    TokenType op;
    struct Node* right;
} BinOpNode;

typedef struct VarDeclNode {
    char *name;
    struct Node *value; // Peut être NULL
} VarDeclNode;

typedef struct IfNode {
    struct Node *condition;
    struct Node **true_block;
    struct Node **false_block; // Peut être NULL
} IfNode;

typedef struct ProgramNode {
    struct Node **statements; // Un tableau de pointeurs vers Node
    int nb_statements;
} ProgramNode;


// Structure de base pour les noeuds
typedef enum {
    NUMBER_NODE, VARREF_NODE, BINOP_NODE, VARDECL_NODE, IF_NODE, PROGRAM_NODE
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        NumberNode numberNode;
        VarRefNode varRefNode;
        BinOpNode binOpNode;
        VarDeclNode varDeclNode;
        IfNode ifNode;
        ProgramNode programNode;
    } data;
} Node;
 

// Déclarations des fonctions du parseur
Token consume(Token *tokens, const int *totalTokens, int *index, TokenType expectedType);
Node* parse_term(Token *tokens, const int *totalTokens, int *index);
Node* parse_expr(Token *tokens, const int *totalTokens, int *index);
Node** parse_block(Token *tokens, const int *totalTokens, int *index);
Node* parse_if(Token *tokens, const int *totalTokens, int *index);
Node* parse_stmt(Token *tokens, const int *totalTokens, int *index);
Node* parse_prog(Token *tokens, int totalTokens);
void print_ast(Node* node, const char* indent, int offset_title, int offset_content);

#endif // PARSER_H