#include <stdlib.h> // malloc()
#include <string.h> // strdup()

#include "node.h"

ParamNode* param_node_new(Char* name, Char* size, I32 line_number)
{
    ParamNode* param_node = (ParamNode*)malloc(sizeof(ParamNode));
    if (!param_node) {
        PANIC("failed to allocate memory");
    }
    param_node->name = strdup(name);
    param_node->size = strdup(size);
    param_node->line_number = line_number;

    return param_node;
}

ExprNode* num_node_new(Char* value, I32 line_number)
{
    ExprNode* num_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!num_node) {
        PANIC("failed to allocate memory");
    }
    num_node->type = NODE_NUM;
    num_node->num_node.value = strdup(value);
    num_node->line_number = line_number;
    return num_node;
}

ExprNode* varref_node_new(Char* name, I32 line_number)
{
    ExprNode* varref_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!varref_node) {
        PANIC("failed to allocate memory");
    }
    varref_node->type = NODE_VARREF;
    varref_node->varref_node.name = strdup(name);
    varref_node->line_number = line_number;
    if (!varref_node->varref_node.name) {
        PANIC("failed to allocate memory");
    }
    return varref_node;
}

ExprNode* addrderef_node_new(ExprNode* expr, I32 line_number)
{
    ExprNode* addrderef_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!addrderef_node) {
        PANIC("failed to allocate memory");
    }
    addrderef_node->type = NODE_ADDRDEREF;
    addrderef_node->addrderef_node.expr = expr;
    addrderef_node->line_number = line_number;

    return addrderef_node;
}

ExprNode* varaddr_node_new(Char* name, I32 line_number)
{
    ExprNode* varaddr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!varaddr_node) {
        PANIC("failed to allocate memory");
    }
    varaddr_node->type = NODE_VARADDR;
    varaddr_node->varaddr_node.name = strdup(name);
    varaddr_node->line_number = line_number;
    if (!varaddr_node->varaddr_node.name) {
        PANIC("failed to allocate memory");
    }
    return varaddr_node;
}

ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right, I32 line_number)
{
    ExprNode* binop_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!binop_node) {
        PANIC("failed to allocate memory");
    }
    binop_node->type = NODE_BINOP;
    binop_node->binop_node.left = left;
    binop_node->binop_node.op = op;
    binop_node->binop_node.right = right;
    binop_node->line_number = line_number;
    return binop_node;
}

ExprNode* unarop_node_new(TokenType op, ExprNode* operand, I32 line_number)
{
    ExprNode* unarop_node = malloc(sizeof(ExprNode));
    if (!unarop_node) {
        PANIC("malloc failed");
    }
    unarop_node->type = NODE_UNARYOP;
    unarop_node->unarop_node.op = op;
    unarop_node->unarop_node.operand = operand;
    unarop_node->line_number = line_number;
    return unarop_node;
}

ExprNode* funcall_node_new(const Char* name, List* expr_node_list, I32 line_number)
{
    ExprNode* expr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }

    expr_node->type = NODE_FUNCALL;
    expr_node->funcall_node.name = strdup(name);
    expr_node->funcall_node.expr_node_list = expr_node_list;
    expr_node->line_number = line_number;

    if (!expr_node->funcall_node.name) {
        PANIC("failed to allocate memory");
    }

    return expr_node;
}

StmtNode* arraydec_node_new(Char* item_size, Char* name, Char* size, List* expr_node_list, I32 line_number)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_ARRAYDEC;
    stmt_node->arraydec_node.item_size = strdup(item_size);
    stmt_node->arraydec_node.name = strdup(name);
    stmt_node->arraydec_node.size = strdup(size);
    stmt_node->arraydec_node.expr_node_list = expr_node_list;
    stmt_node->line_number = line_number;

    return stmt_node;
}

StmtNode* vardec_node_new(Char* size, Char* name, ExprNode* value, I32 line_number)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_VARDEC;
    stmt_node->vardec_node.size = strdup(size);
    stmt_node->vardec_node.name = strdup(name);
    stmt_node->vardec_node.value = value;
    stmt_node->line_number = line_number;

    return stmt_node;
}

StmtNode* if_node_new(ExprNode* cond_node, BlockNode* true_block, BlockNode* false_block, I32 line_number)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_IF;
    stmt_node->if_node.cond_node = cond_node;
    stmt_node->if_node.true_block = true_block;
    stmt_node->if_node.false_block = false_block;
    stmt_node->line_number = line_number;

    return stmt_node;
}

StmtNode* loop_node_new(BlockNode* block, I32 line_number)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_LOOP;
    stmt_node->loop_node.block = block;
    stmt_node->line_number = line_number;

    return stmt_node;
}

StmtNode* break_node_new(I32 line_number)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_BREAK;
    stmt_node->break_node.nothing = NULL;
    stmt_node->line_number = line_number;

    return stmt_node;
}

StmtNode* ass_node_new(ExprNode* lvalue, ExprNode* value, I32 line_number)
{
    StmtNode* ass_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!ass_node) {
        PANIC("failed to allocate memory");
    }

    ass_node->type = NODE_ASS;
    ass_node->ass_node.lvalue = lvalue;
    ass_node->ass_node.value = value;
    ass_node->line_number = line_number;

    return ass_node;
}

StmtNode* ret_node_new(ExprNode* expr_node, I32 line_number)
{
    StmtNode* ret_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!ret_node) {
        PANIC("failed to allocate memory");
    }

    ret_node->type = NODE_RET;
    ret_node->ret_node.expr_node = expr_node;
    ret_node->line_number = line_number;

    if (!ret_node->ret_node.expr_node) {
        PANIC("failed to allocate memory");
    }

    return ret_node;
}

ExprNode* sysc_node_new(List* expr_node_list, I32 line_number)
{
    ExprNode* sysc_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!sysc_node) {
        PANIC("failed to allocate memory");
    }

    sysc_node->type = NODE_SYSC;
    sysc_node->sysc_node.expr_node_list = expr_node_list;
    sysc_node->line_number = line_number;

    return sysc_node;
}

FundefNode* fundef_node_new(Char* name, Char* return_size, Char* scope, List* param_node_list, BlockNode* block_node, I32 line_number)
{
    FundefNode* fundef_node = (FundefNode*)malloc(sizeof(FundefNode));
    if (!fundef_node) {
        PANIC("failed to allocate memory");
    }

    fundef_node->name = strdup(name);

    if (return_size == NULL) {
        fundef_node->return_size = strdup("NULL");
    }
    else {
        fundef_node->return_size = strdup(return_size);
    }

    fundef_node->scope = strdup(scope);
    fundef_node->param_node_list = param_node_list;
    fundef_node->block_node = block_node;
    fundef_node->line_number = line_number;

    return fundef_node;
}

BlockNode* block_node_new(List* stmt_node_list, I32 line_number)
{
    BlockNode* block_node = (BlockNode*)malloc(sizeof(BlockNode));
    if (!block_node) {
        PANIC("failed to allocate memory");
    }
    block_node->stmt_node_list = stmt_node_list;
    block_node->line_number = line_number;
    return block_node;
}
