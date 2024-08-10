#include "node.h"
#include <stdlib.h> // malloc()
#include <string.h> // strdup()

ParamNode* param_node_new(Char* mut, Char* name, Char* type)
{
    ParamNode* param_node = (ParamNode*)malloc(sizeof(ParamNode));
    if (!param_node) {
        PANIC("failed to allocate memory");
    }
    param_node->mut = mut;
    param_node->name = strdup(name);
    param_node->type = strdup(type);
    if (!param_node->name || !param_node->type) {
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
    num_node->type = NODE_NUMBER;
    num_node->num_node.value = strdup(value);
    return num_node;
}

ExprNode* varref_node_new(Char* name)
{
    ExprNode* varref_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!varref_node) {
        PANIC("failed to allocate memory");
    }
    varref_node->type = NODE_VAR_REF;
    varref_node->varref_node.name = strdup(name);
    if (!varref_node->varref_node.name) {
        PANIC("failed to allocate memory");
    }
    return varref_node;
}

ExprNode* varaddr_node_new(Char* name)
{
    ExprNode* varaddr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!varaddr_node) {
        PANIC("failed to allocate memory");
    }
    varaddr_node->type = NODE_VAR_ADDR;
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

    expr_node->type = NODE_FUN_CALL;
    expr_node->funcall_node.name = strdup(name);
    expr_node->funcall_node.expr_node_list = expr_node_list;

    if (!expr_node->funcall_node.name) {
        PANIC("failed to allocate memory");
    }

    return expr_node;
}

StmtNode* vardef_node_new(Char* mut, const Char* type, const Char* name, ExprNode* value)
{
    StmtNode* vardef_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!vardef_node) {
        PANIC("failed to allocate memory");
    }

    vardef_node->type = NODE_VARDEF;
    vardef_node->vardef_node.mut = mut;
    vardef_node->vardef_node.type = strdup(type);
    vardef_node->vardef_node.name = strdup(name);
    vardef_node->vardef_node.value = value;

    if (!vardef_node->vardef_node.type || !vardef_node->vardef_node.name) {
        PANIC("failed to allocate memory");
    }

    return vardef_node;
}

StmtNode* varmod_node_new(const Char* name, ExprNode* value)
{
    StmtNode* varmod_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!varmod_node) {
        PANIC("failed to allocate memory");
    }

    varmod_node->type = NODE_VARMOD;
    varmod_node->varmod_node.name = strdup(name);
    varmod_node->varmod_node.value = value;

    if (!varmod_node->varmod_node.name) {
        PANIC("failed to allocate memory");
    }

    return varmod_node;
}

StmtNode* return_node_new(Bool is_empty, ExprNode* expr_node)
{
    StmtNode* return_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!return_node) {
        PANIC("failed to allocate memory");
    }

    return_node->type = NODE_RETURN;
    return_node->return_node.is_empty = is_empty;
    return_node->return_node.expr_node = expr_node;

    // if (!return_node->return_node.expr_node) {
    //     PANIC("failed to allocate memory");
    // }
    // The above line was removed because of the fact that an expression can be NULL for the return statement

    return return_node;
}

StmtNode* syscall_node_new(List* expr_node_list)
{
    StmtNode* syscall_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!syscall_node) {
        PANIC("failed to allocate memory");
    }

    syscall_node->type = NODE_SYSCALL;
    syscall_node->syscall_node.expr_node_list = expr_node_list;

    return syscall_node;
}

DefNode* fundef_node_new(Char* name, Char* return_type, List* param_node_list, CodeblockNode* codeblock_node)
{
    DefNode* def_node = (DefNode*)malloc(sizeof(DefNode));
    if (!def_node) {
        PANIC("failed to allocate memory");
    }

    def_node->type = NODE_FUNDEF;
    def_node->fundef_node.name = strdup(name);
    def_node->fundef_node.return_type = strdup(return_type);
    def_node->fundef_node.param_node_list = param_node_list;
    def_node->fundef_node.codeblock_node = codeblock_node;

    if (!def_node->fundef_node.name || !def_node->fundef_node.return_type) {
        PANIC("failed to allocate memory");
    }

    return def_node;
}

DefNode* startdef_node_new(CodeblockNode* codeblock_node)
{
    DefNode* def_node = (DefNode*)malloc(sizeof(DefNode));
    if (!def_node) {
        PANIC("failed to allocate memory");
    }

    def_node->type = NODE_STARTDEF;
    def_node->startdef_node.codeblock_node = codeblock_node;

    return def_node;
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