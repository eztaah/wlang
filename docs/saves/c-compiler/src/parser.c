#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"


// ------------------------------------------------------------

// Renoie le token actuel si c'est celui qui est attendu. Incremente l'index
Token consume(Token *tokens, const int *totalTokens, int *index, TokenType expectedType) 
{
    if (*index < *totalTokens) {
        Token currentToken = tokens[*index];
        (*index)++;

        // Si le type du token n'est pas celui attendu
        if (expectedType != currentToken.type) {
            printf("Expected token type %i, got (%i, %s)", expectedType, currentToken.type, currentToken.value);
            exit(1); 
        }
        return currentToken;
    }
    // Si il n'y a plus de token
    else {
        printf("No more tokens");
        exit(1);
    }
}


Node *parse_term(Token *tokens, const int *totalTokens, int *index) 
{

    if (tokens[*index].type == NUMBER) {

        TokenType expectedType = NUMBER;
        // recupere le token actuel, on s'attend à ce que ce soit NUMBER
        Token token = consume(tokens, totalTokens, index, expectedType);

        // On return une numberNode
        Node* node = (Node*)malloc(sizeof(Node));
        node->type = NUMBER_NODE;
        node->data.numberNode.value = token.value;
        return node;
    }

    if (tokens[*index].type == IDENTIFIER) {

        TokenType expectedType = IDENTIFIER;
        // recupere le token actuel, on s'attend à ce que ce soit NUMBER
        Token token = consume(tokens, totalTokens, index, expectedType);

        // On return une numberNode
        Node* node = (Node*)malloc(sizeof(Node));
        node->type = VARREF_NODE;
        node->data.varRefNode.name = token.value;
        return node;
    }
    else {
        printf("error");
        exit(1);
    }
}


Node *parse_expr(Token *tokens, const int *totalTokens, int *index) {
    Node *node = parse_term(tokens, totalTokens, index);

    while (*index < *totalTokens && 
           (tokens[*index].type == PLUS || tokens[*index].type == TIMES || tokens[*index].type == EQUALS_EQUALS)) {
        TokenType opType = tokens[*index].type;
        consume(tokens, totalTokens, index, opType); // Consomme l'opérateur
        
        Node *right = parse_term(tokens, totalTokens, index);
        
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->type = BINOP_NODE;
        newNode->data.binOpNode.op = opType;
        newNode->data.binOpNode.left = node;
        newNode->data.binOpNode.right = right;
        
        node = newNode; // Met à jour le nœud actuel pour être le nœud de l'opération
    }
    
    return node;
}


Node** parse_block(Token *tokens, const int *totalTokens, int *index) {
    consume(tokens, totalTokens, index, LBRACE);
    
    Node** statements = malloc(sizeof(Node*) * 100); // Taille arbitraire, ajustez selon vos besoins
    int statementsCount = 0;
    
    while (*index < *totalTokens && tokens[*index].type != RBRACE) {
        if (statementsCount >= 100) {
            printf("Error: Maximum statement capacity reached.\n");
            exit(1);
        }
        
        statements[statementsCount++] = parse_stmt(tokens, totalTokens, index);
    }
    
    consume(tokens, totalTokens, index, RBRACE);
    
    // Ajuster la taille du tableau pour correspondre exactement au nombre de déclarations
    statements = realloc(statements, sizeof(Node*) * statementsCount);
    
    // Retourner un pointeur vers le tableau de déclarations
    return statements;
}


Node* parse_if(Token *tokens, const int *totalTokens, int *index) {
    consume(tokens, totalTokens, index, IF);
    consume(tokens, totalTokens, index, LPAREN);
    
    Node* condition = parse_expr(tokens, totalTokens, index);
    
    consume(tokens, totalTokens, index, RPAREN);
    
    Node** true_branch = parse_block(tokens, totalTokens, index);
    
    // Création du nœud If
    Node* ifNode = (Node*)malloc(sizeof(Node));
    ifNode->type = IF_NODE;
    ifNode->data.ifNode.condition = condition;
    ifNode->data.ifNode.true_block = true_branch;
    ifNode->data.ifNode.false_block = NULL; // Pas de prise en charge de else pour le moment
    
    return ifNode;
}


Node* parse_stmt(Token *tokens, const int *totalTokens, int *index) {
    // Analyse d'une instruction if
    if (tokens[*index].type == IF) {
        return parse_if(tokens, totalTokens, index);
    }
    
    // Analyse d'une déclaration/affectation de variable
    if (tokens[*index].type == IDENTIFIER && tokens[*index + 1].type == EQUALS) {
        Token varNameToken = consume(tokens, totalTokens, index, IDENTIFIER);
        
        consume(tokens, totalTokens, index, EQUALS);
        
        Node* value = parse_expr(tokens, totalTokens, index);
        
        consume(tokens, totalTokens, index, SEMICOLON);
        
        // Création d'un nœud pour la déclaration de variable
        Node* varDeclNode = (Node*)malloc(sizeof(Node));
        varDeclNode->type = VARDECL_NODE;
        varDeclNode->data.varDeclNode.name = varNameToken.value;
        varDeclNode->data.varDeclNode.value = value;
        
        return varDeclNode;
    }
    
    // Ajouter ici la gestion d'autres types d'instructions si nécessaire
    printf("Error: Unexpected token type in parse_stmt.\n");
    exit(1); // Terminaison pour erreur inattendue, à gérer plus élégamment dans un vrai compilateur
}


Node* parse_prog(Token *tokens, int totalTokens) {
    int index = 0; // Initialise l'index pour parcourir les tokens
    
    // Utilisation d'une capacité fixe pour le tableau des instructions
    int capacity = 100; // Valeur fixe pour simplifier
    Node** statements = (Node**)malloc(capacity * sizeof(Node*));
    if (!statements) {
        printf("Memory allocation error\n");
        exit(1);
    }
    
    int statementsCount = 0;
    
    while (index < totalTokens && tokens[index].type != EOF_TOKEN) {
        statements[statementsCount++] = parse_stmt(tokens, &totalTokens, &index);
        // Ici, pas de vérification de débordement pour simplifier, mais attention en production
    }
    
    // Création du nœud du programme sans ajuster la taille du tableau
    Node* progNode = (Node*)malloc(sizeof(Node));
    progNode->type = PROGRAM_NODE;
    progNode->data.programNode.statements = statements;
    progNode->data.programNode.nb_statements = statementsCount;
    
    return progNode;
}


void print_indent(const char* indent) {
    printf("%s", indent);
}


void print_ast(Node* node, const char* indent, int offset_title, int offset_content) {
    
    char branch[10];
    // Offset for the title
    if (offset_title == 1) {
        strcpy(branch, "");
    }
    else if (offset_title == 2) {
        strcpy(branch, "│  ");
    }
    else if (offset_title == 3) {
        strcpy(branch, "     ");
    }
    else if (offset_title == 4) {
        strcpy(branch, "   ");
    }
    else if (offset_title == 5) {
        strcpy(branch, "   ");
    }
    else {
        strcpy(branch, offset_content ? "└─ " : "├─ ");
    }


    // offset for the content
    char next_indent[256];
    if (offset_content == 1) {
        sprintf(next_indent, "%s  ", indent);
    } 
    else if (offset_content == 2) {
        sprintf(next_indent, "%s        ", indent);
    }
    else {
        sprintf(next_indent, "%s│  ", indent);
    }
    




    switch (node->type) {
        case PROGRAM_NODE:
            print_indent(indent);
            printf("ProgramNode\n");
            for (int i = 0; i < node->data.programNode.nb_statements; i++) {
                print_ast(node->data.programNode.statements[i], next_indent, 0, i == (node->data.programNode.nb_statements - 1));
                printf("  │\n");
            }
            break;
        case IF_NODE:
            print_indent(indent);
            printf("%sIfNode\n", branch);
            print_indent(next_indent);
            printf("├─ condition: \n");
            print_ast(node->data.ifNode.condition, next_indent, 3, 2);
            if (node->data.ifNode.true_block != NULL) {
                print_indent(next_indent);
                printf("└─ true_block: \n");
                for (int i = 0; node->data.ifNode.true_block[i] != NULL; i++) {
                    print_ast(node->data.ifNode.true_block[i], next_indent, 0, 1);
                }
            } else {
                print_indent(next_indent);
                printf("└── true_block: \n");
            }
            break;
        case BINOP_NODE:
            print_indent(indent);
            printf("%sBinOpNode\n", branch);
            print_indent(next_indent);
            printf("├─ op: %d\n", node->data.binOpNode.op);
            print_indent(next_indent);
            printf("├─ left: \n");
            print_ast(node->data.binOpNode.left, next_indent, 2, 0);
            print_indent(next_indent);
            printf("└─ right: \n");
            print_ast(node->data.binOpNode.right, next_indent, 4, 1);
            break;
        case NUMBER_NODE:
            print_indent(indent);
            printf("%sNumberNode(value=%s)\n", branch, node->data.numberNode.value);
            break;
        case VARDECL_NODE:
            print_indent(indent);
            printf("%sVarDeclNode\n", branch);
            print_indent(next_indent);
            printf("  ├─ name: %s\n", node->data.varDeclNode.name);
            print_indent(next_indent);
            printf("  └─ value:\n");

            if (node->data.varDeclNode.value != NULL) {
                print_ast(node->data.varDeclNode.value, next_indent, 3, 2);
            }
            break;
        case VARREF_NODE:
            print_indent(indent);
            printf("%sVarRefNode(name=%s)\n", branch, node->data.varRefNode.name);
            break;
        default:
            fprintf(stderr, "Unknown node type\n");
            exit(1);
    }
}
