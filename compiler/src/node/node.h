#ifndef NODE_H
#define NODE_H

#include "lib.h"
#include "token.h"

typedef struct {
    Char* mut;
    Char* name;
    Char* type;
} ParamNode;

// OTHER NODE

typedef struct ExprNode ExprNode;

typedef struct {
    Char* value;
} NumberNode;

typedef struct {
    Char* name;
} VarRefNode;

typedef struct {
    Char* name;
} VarAddrNode;

typedef struct {
    TokenType op;
    ExprNode* operand;
} UnaryOpNode;

typedef struct {
    ExprNode* left;
    TokenType op;
    ExprNode* right;
} BinopNode;

typedef struct {
    Char* name;
    List* expr_node_list;
} FunCallNode;

struct ExprNode {
    enum {
        NODE_NUMBER = 10,
        NODE_VAR_REF = 11,
        NODE_VAR_ADDR = 12,
        NODE_BINOP = 13,
        NODE_UNARYOP = 14,
        NODE_FUN_CALL = 15,
    } type;
    union {
        BinopNode bin_op_node;
        UnaryOpNode unary_op_node;
        VarRefNode var_ref_node;
        VarAddrNode var_addr_node;
        NumberNode number_node;
        FunCallNode fun_call_node;
    };
};

// EXPRESSION NODE


typedef struct {
    Char* mut;
    Char* name;
    Char* type;
    ExprNode* value;
} VarDefNode;

typedef struct {
    Char* name;
    ExprNode* value;
} VarModifNode;

typedef struct {
    Bool is_empty;     // not the right way of doing it but ok for now
    ExprNode* expr_node;
} ReturnNode;

typedef struct {
    List* expr_node_list;
} SyscallNode;

typedef struct {
    enum {
        NODE_VAR_DEF,
        NODE_VAR_MODIF,
        NODE_RETURN,
        NODE_SYSCALL,
        NODE_EXPR
    } type;
    union {
        VarDefNode var_def;
        VarModifNode var_modif;
        ReturnNode return_node;
        SyscallNode syscall_node;
        ExprNode expr_node;
    };
} InstrNode;

// INSTRUCTIONS NODE


typedef struct {
    List* instr_node_list;
} CodeblockNode;

typedef struct {
    Char* name;
    Char* return_type;
    List* param_node_list;
    CodeblockNode* code_block;
} FunDefNode;

typedef struct {
    CodeblockNode* code_block;
} StartNode;

typedef struct {
    enum {
        NODE_FUN_DEF,
        NODE_START
    } type;
    union {
        FunDefNode fun_def_node;
        StartNode start_node;
    };
} StmtNode;

// STATEMENT NODE


ParamNode* param_node_new(Char* mut, Char* name, Char* type);

ExprNode* number_node_new(Char* value);
ExprNode* var_ref_node_new(Char* name);
ExprNode* var_addr_node_new(Char* name);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right);
ExprNode* unaryop_node_new(TokenType op, ExprNode* operand);
ExprNode* fun_call_node_new(const Char* name, List* expr_node_list);

InstrNode* var_def_node_new(Char* mut, const Char* type, const Char* name, ExprNode* value);
InstrNode* var_modif_node_new(const Char* name, ExprNode* value);
InstrNode* return_node_new(Bool is_empty, ExprNode* expr_node);
InstrNode* syscall_node_new(List* expr_node_list);

StmtNode* fun_def_node_new(Char* name, Char* return_type, List* param_node_list, CodeblockNode* code_block);
StmtNode* start_node_new(CodeblockNode* code_block);

CodeblockNode* code_block_new(List* stmt_node_list);

Str* print_nodelist(const List* node_list);

#endif
