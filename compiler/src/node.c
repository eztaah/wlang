#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // For perror

// Instantiate a NumberNode wrapped in an ExpressionNode
ExpressionNode* instanciate_number_node(Char* value) {
    ExpressionNode* expr_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
    if (!expr_node) {
        perror("Failed to allocate ExpressionNode for NumberNode");
        exit(EXIT_FAILURE);
    }
    expr_node->type = NODE_NUMBER;
    expr_node->number_node.value = strdup(value);
    return expr_node;
}

// Instantiate a BinOpNode wrapped in an ExpressionNode
ExpressionNode* instanciate_binop_node(ExpressionNode* left, TokenType op, ExpressionNode* right) {
    ExpressionNode* expr_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
    if (!expr_node) {
        perror("Failed to allocate ExpressionNode for BinOpNode");
        exit(EXIT_FAILURE);
    }
    expr_node->type = NODE_BINOP;
    expr_node->bin_op_node.left = left;
    expr_node->bin_op_node.op = op;
    expr_node->bin_op_node.right = right;
    return expr_node;
}

// Instantiate a VarDeclNode
VarDeclNode* instanciate_var_decl_node(const Char* type, const Char* name, ExpressionNode* value) {
    VarDeclNode* node = (VarDeclNode*)malloc(sizeof(VarDeclNode));
    if (!node) {
        perror("Failed to allocate VarDeclNode");
        exit(EXIT_FAILURE);
    }
    node->type = strdup(type);
    if (!node->type) {
        perror("Failed to allocate VarDeclNode type");
        exit(EXIT_FAILURE);
    }
    node->name = strdup(name);
    if (!node->name) {
        perror("Failed to allocate VarDeclNode name");
        exit(EXIT_FAILURE);
    }
    node->value = value;
    return node;
}




// Helper function for string concatenation
static Char* strcat_dynamic(Char* dest, const Char* src) {
    size_t dest_len = dest ? strlen(dest) : 0;
    size_t src_len = src ? strlen(src) : 0;
    Char* result = malloc(dest_len + src_len + 1);
    if (result) {
        if (dest) {
            strcpy(result, dest);
        }
        if (src) {
            strcpy(result + dest_len, src);
        }
    }
    // free(dest);
    return result;
}


Char* token_to_string2(TokenType token_type)
{
    switch (token_type) {
        case TOKEN_PLUS:
            return "TOKEN_PLUS";
            break;
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
            break;
        case TOKEN_DIV:
            return "TOKEN_DIV";
            break;
        case TOKEN_MUL:
            return "TOKEN_MUL";
            break;
        case TOKEN_INT:
            return "TOKEN_INT";
            break;
        case TOKEN_CST:
            return "TOKEN_CST";
            break;
        case TOKEN_VAR:
            return "TOKEN_VAR";
            break;
        case TOKEN_ID:
            return "TOKEN_ID";
            break;
        case TOKEN_TYPE:
            return "TOKEN_TYPE";
            break;
        case TOKEN_COLON:
            return "TOKEN_COLON";
            break;
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
            break;
        case TOKEN_END_STATEMENT:
            return "TOKEN_END_STATEMENT";
            break;
        case TOKEN_EOF:
            return "TOKEN_EOF";
            break;
        default:
            printf("in token_to_string(): a token does not have string equivalent");
            abort();
    }
}

// Print Expression Node
Char* print_expression_node(const ExpressionNode* node, Char* output, const Char* indent, B32 last, B32 is_value) {
    const char* branch = is_value ? "" : (last ? "└─ " : "├─ ");
    Char* next_indent = strcat_dynamic(strdup(indent), last ? "   " : "│  ");

    switch (node->type) {
        case NODE_NUMBER: {
            NumberNode number_node = node->number_node;
            output = strcat_dynamic(output, strcat_dynamic(strdup(indent), branch));
            output = strcat_dynamic(output, "NumberNode(value=");
            output = strcat_dynamic(output, number_node.value);
            output = strcat_dynamic(output, ")\n");
            break;
        }
        case NODE_BINOP: {
            BinOpNode bin_op_node = node->bin_op_node;
            output = strcat_dynamic(output, strcat_dynamic(strdup(indent), branch));
            output = strcat_dynamic(output, "BinOpNode\n");
            output = strcat_dynamic(output, strcat_dynamic(next_indent, "├─ op: "));
            output = strcat_dynamic(output, token_to_string2(bin_op_node.op)); // Ensure token_to_string accepts TokenType
            output = strcat_dynamic(output, "\n");
            output = strcat_dynamic(output, strcat_dynamic(next_indent, "├─ left: \n"));
            output = print_expression_node(bin_op_node.left, output, strcat_dynamic(next_indent, "│  "), false, true);
            output = strcat_dynamic(output, strcat_dynamic(next_indent, "└─ right: \n"));
            output = print_expression_node(bin_op_node.right, output, strcat_dynamic(next_indent, "   "), true, true);
            break;
        }
        default:
            fprintf(stderr, "Unknown expression node type encountered while displaying the AST\n");
            exit(1);
    }

    free(next_indent);
    return output;
}


// Print AST
Char* print_ast(const StatementNode* node, Char* output, const Char* indent, B32 last, B32 is_value) {
    const char* branch = is_value ? "" : (last ? "└─ " : "├─ ");
    Char* next_indent = strcat_dynamic(strdup(indent), last ? "   " : "│  ");

    switch (node->type) {
        case NODE_VAR_DECL: {
            VarDeclNode var_decl = node->var_decl;
            output = strcat_dynamic(output, strcat_dynamic(strdup(indent), branch));
            output = strcat_dynamic(output, "VarDeclNode\n");
            output = strcat_dynamic(output, strcat_dynamic(next_indent, "├─ type: "));
            output = strcat_dynamic(output, var_decl.type);
            output = strcat_dynamic(output, "\n");
            output = strcat_dynamic(output, strcat_dynamic(next_indent, "├─ name: "));
            output = strcat_dynamic(output, var_decl.name);
            output = strcat_dynamic(output, "\n");
            output = strcat_dynamic(output, strcat_dynamic(next_indent, "└─ value: \n"));
            if (var_decl.value) {
                output = print_expression_node(var_decl.value, output, strcat_dynamic(next_indent, "   "), true, true);
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown node type encountered while displaying the AST\n");
            exit(1);
    }

    free(next_indent);
    return output;
}

