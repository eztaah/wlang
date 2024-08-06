#include "node.h"
#include <stdio.h> // For perror
#include <stdlib.h>
#include <string.h>

ExpressionNode* instanciate_number_node(Char* value)
{
    ExpressionNode* expr_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }
    expr_node->type = NODE_NUMBER;
    expr_node->number_node.value = strdup(value);
    return expr_node;
}

ExpressionNode* instanciate_binop_node(ExpressionNode* left, TokenType op, ExpressionNode* right)
{
    ExpressionNode* expr_node = (ExpressionNode*)malloc(sizeof(ExpressionNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }
    expr_node->type = NODE_BINOP;
    expr_node->bin_op_node.left = left;
    expr_node->bin_op_node.op = op;
    expr_node->bin_op_node.right = right;
    return expr_node;
}

StatementNode* instanciate_var_decl_node(const Char* type, const Char* name, ExpressionNode* value)
{
    StatementNode* stmt_node = (StatementNode*)malloc(sizeof(StatementNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_VAR_DECL;

    stmt_node->var_decl.type = strdup(type);
    if (!stmt_node->var_decl.type) {
        PANIC("failed to allocate memory");
    }
    stmt_node->var_decl.name = strdup(name);
    if (!stmt_node->var_decl.name) {
        PANIC("failed to allocate memory");
    }

    stmt_node->var_decl.value = value;
    return stmt_node;
}
