#include "node.h"
#include <stdio.h> // For perror
#include <stdlib.h>
#include <string.h>

ExprNode* number_node_new(Char* value)
{
    ExprNode* expr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }
    expr_node->type = NODE_NUMBER;
    expr_node->number_node.value = strdup(value);
    return expr_node;
}

ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right)
{
    ExprNode* expr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }
    expr_node->type = NODE_BINOP;
    expr_node->bin_op_node.left = left;
    expr_node->bin_op_node.op = op;
    expr_node->bin_op_node.right = right;
    return expr_node;
}

StmtNode* var_decl_node_new(const Char* type, const Char* name, ExprNode* value)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
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
