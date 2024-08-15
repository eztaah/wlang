#include <stdlib.h> // malloc()
#include <string.h> // strdup()

#include "node.h"

ParamNode* param_node_new(Char* name)
{
    ParamNode* param_node = (ParamNode*)malloc(sizeof(ParamNode));
    if (!param_node) {
        PANIC("failed to allocate memory");
    }
    param_node->name = strdup(name);
    if (!param_node->name) {
        PANIC("failed to allocate memory");
    }
    return param_node;
}

ExprNode* num_node_new(Char* value)
{
    ExprNode* num_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!num_node) {
        PANIC("failed to allocate memory");
    }
    num_node->type = NODE_NUM;
    num_node->num_node.value = strdup(value);
    return num_node;
}

ExprNode* varref_node_new(Char* name)
{
    ExprNode* varref_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!varref_node) {
        PANIC("failed to allocate memory");
    }
    varref_node->type = NODE_VARREF;
    varref_node->varref_node.name = strdup(name);
    if (!varref_node->varref_node.name) {
        PANIC("failed to allocate memory");
    }
    return varref_node;
}

ExprNode* addrderef_node_new(ExprNode* expr)
{
    ExprNode* addrderef_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!addrderef_node) {
        PANIC("failed to allocate memory");
    }
    addrderef_node->type = NODE_ADDRDEREF;
    addrderef_node->addrderef_node.expr = expr;

    return addrderef_node;
}

ExprNode* varaddr_node_new(Char* name)
{
    ExprNode* varaddr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!varaddr_node) {
        PANIC("failed to allocate memory");
    }
    varaddr_node->type = NODE_VARADDR;
    varaddr_node->varaddr_node.name = strdup(name);
    if (!varaddr_node->varaddr_node.name) {
        PANIC("failed to allocate memory");
    }
    return varaddr_node;
}

ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right)
{
    ExprNode* binop_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!binop_node) {
        PANIC("failed to allocate memory");
    }
    binop_node->type = NODE_BINOP;
    binop_node->binop_node.left = left;
    binop_node->binop_node.op = op;
    binop_node->binop_node.right = right;
    return binop_node;
}

ExprNode* unarop_node_new(TokenType op, ExprNode* operand)
{
    ExprNode* unarop_node = malloc(sizeof(ExprNode));
    if (!unarop_node) {
        PANIC("malloc failed");
    }
    unarop_node->type = NODE_UNARYOP;
    unarop_node->unarop_node.op = op;
    unarop_node->unarop_node.operand = operand;
    return unarop_node;
}

ExprNode* funcall_node_new(const Char* name, List* expr_node_list)
{
    ExprNode* expr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }

    expr_node->type = NODE_FUNCALL;
    expr_node->funcall_node.name = strdup(name);
    expr_node->funcall_node.expr_node_list = expr_node_list;

    if (!expr_node->funcall_node.name) {
        PANIC("failed to allocate memory");
    }

    return expr_node;
}

StmtNode* varass_node_new(ExprNode* lvalue, ExprNode* value)
{
    StmtNode* varass_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!varass_node) {
        PANIC("failed to allocate memory");
    }

    varass_node->type = NODE_VARASS;
    varass_node->varass_node.lvalue = lvalue;
    varass_node->varass_node.value = value;

    return varass_node;
}

StmtNode* ret_node_new(ExprNode* expr_node)
{
    StmtNode* ret_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!ret_node) {
        PANIC("failed to allocate memory");
    }

    ret_node->type = NODE_RET;
    ret_node->ret_node.expr_node = expr_node;

    if (!ret_node->ret_node.expr_node) {
        PANIC("failed to allocate memory");
    }

    return ret_node;
}

ExprNode* sysc_node_new(List* expr_node_list)
{
    ExprNode* sysc_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!sysc_node) {
        PANIC("failed to allocate memory");
    }

    sysc_node->type = NODE_SYSC;
    sysc_node->sysc_node.expr_node_list = expr_node_list;

    return sysc_node;
}

FundefNode* fundef_node_new(Char* name, Char* scope, List* param_node_list, CodeblockNode* codeblock_node)
{
    FundefNode* fundef_node = (FundefNode*)malloc(sizeof(FundefNode));
    if (!fundef_node) {
        PANIC("failed to allocate memory");
    }

    fundef_node->name = strdup(name);
    fundef_node->scope = strdup(scope);
    fundef_node->param_node_list = param_node_list;
    fundef_node->codeblock_node = codeblock_node;

    return fundef_node;
}

CodeblockNode* codeblock_node_new(List* stmt_node_list)
{
    CodeblockNode* codeblock_node = (CodeblockNode*)malloc(sizeof(CodeblockNode));
    if (!codeblock_node) {
        PANIC("failed to allocate memory");
    }
    codeblock_node->stmt_node_list = stmt_node_list;
    return codeblock_node;
}