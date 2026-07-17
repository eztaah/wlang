#include "node.h"
#include <stdio.h> // For perror
#include <stdlib.h>
#include <string.h>

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

ExprNode* var_ref_node_new(Char* name)
{
    ExprNode* expr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!expr_node) {
        PANIC("failed to allocate memory");
    }
    expr_node->type = NODE_VAR_REF;
    expr_node->var_ref_node.name = strdup(name);
    if (!expr_node->var_ref_node.name) {
        PANIC("failed to allocate memory");
    }
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

ExprNode* unaryop_node_new(TokenType op, ExprNode* operand)
{
    ExprNode* node = malloc(sizeof(ExprNode));
    if (!node) {
        PANIC("malloc failed");
    }
    node->type = NODE_UNARYOP;
    node->unary_op_node.op = op;
    node->unary_op_node.operand = operand;
    return node;
}

ExprNode* fun_call_node_new(const Char* name, List* expr_node_list)
{
    ExprNode* instr_node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!instr_node) {
        PANIC("failed to allocate memory");
    }

    instr_node->type = NODE_FUN_CALL;
    instr_node->fun_call_node.name = strdup(name);
    instr_node->fun_call_node.expr_node_list = expr_node_list;

    if (!instr_node->fun_call_node.name) {
        PANIC("failed to allocate memory");
    }

    return instr_node;
}

InstrNode* var_def_node_new(Char* mut, const Char* type, const Char* name, ExprNode* value)
{
    InstrNode* instr_node = (InstrNode*)malloc(sizeof(InstrNode));
    if (!instr_node) {
        PANIC("failed to allocate memory");
    }

    instr_node->type = NODE_VAR_DEF;
    instr_node->var_def.mut = mut;
    instr_node->var_def.type = strdup(type);
    instr_node->var_def.name = strdup(name);
    instr_node->var_def.value = value;

    if (!instr_node->var_def.type || !instr_node->var_def.name) {
        PANIC("failed to allocate memory");
    }

    return instr_node;
}

InstrNode* var_modif_node_new(const Char* name, ExprNode* value)
{
    InstrNode* instr_node = (InstrNode*)malloc(sizeof(InstrNode));
    if (!instr_node) {
        PANIC("failed to allocate memory");
    }

    instr_node->type = NODE_VAR_MODIF;
    instr_node->var_modif.name = strdup(name);
    instr_node->var_modif.value = value;

    if (!instr_node->var_modif.name) {
        PANIC("failed to allocate memory");
    }

    return instr_node;
}

InstrNode* return_node_new(ExprNode* expr_node)
{
    InstrNode* instr_node = (InstrNode*)malloc(sizeof(InstrNode));
    if (!instr_node) {
        PANIC("failed to allocate memory");
    }

    instr_node->type = NODE_RETURN;
    instr_node->return_node.expr_node = expr_node;

    if (!instr_node->return_node.expr_node) {
        PANIC("failed to allocate memory");
    }

    return instr_node;
}

InstrNode* syscallwrite_node_new(Char* char_location_ptr_name)
{
    InstrNode* instr_node = (InstrNode*)malloc(sizeof(InstrNode));
    if (!instr_node) {
        PANIC("failed to allocate memory");
    }

    instr_node->type = NODE_SYSCALLWRITE;
    instr_node->syscallwrite_node.char_location_ptr_name = char_location_ptr_name;

    return instr_node;
}

StmtNode* fun_def_node_new(Char* name, Char* return_type, List* param_node_list, CodeblockNode* code_block)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_FUN_DEF;
    stmt_node->fun_def_node.name = strdup(name);
    stmt_node->fun_def_node.return_type = strdup(return_type);
    stmt_node->fun_def_node.param_node_list = param_node_list;
    stmt_node->fun_def_node.code_block = code_block;

    if (!stmt_node->fun_def_node.name || !stmt_node->fun_def_node.return_type) {
        PANIC("failed to allocate memory");
    }

    return stmt_node;
}

StmtNode* start_node_new(CodeblockNode* code_block)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }

    stmt_node->type = NODE_START;
    stmt_node->start_node.code_block = code_block;

    return stmt_node;
}

CodeblockNode* code_block_new(List* stmt_node_list)
{
    CodeblockNode* code_block = (CodeblockNode*)malloc(sizeof(CodeblockNode));
    if (!code_block) {
        PANIC("failed to allocate memory");
    }
    code_block->stmt_node_list = stmt_node_list;
    return code_block;
}