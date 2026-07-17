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
        NODE_BINOP = 12,
        NODE_UNARYOP = 13,
        NODE_FUN_CALL = 14,
    } type;
    union {
        BinopNode bin_op_node;
        UnaryOpNode unary_op_node;
        VarRefNode var_ref_node;
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
    ExprNode* expr_node;
} ReturnNode;

typedef struct {
    Char* char_location_ptr_name;
} SyscallwriteNode;

typedef struct {
    enum {
        NODE_VAR_DEF,
        NODE_VAR_MODIF,
        NODE_RETURN,
        NODE_SYSCALLWRITE,
        NODE_EXPR
    } type;
    union {
        VarDefNode var_def;
        VarModifNode var_modif;
        ReturnNode return_node;
        SyscallwriteNode syscallwrite_node;
        ExprNode expr_node;
    };
} InstrNode;

// INSTRUCTIONS NODE


typedef struct {
    List* stmt_node_list;
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
        NODE_INSTR,
        NODE_FUN_DEF,
        NODE_START
    } type;
    union {
        InstrNode instr_node;
        FunDefNode fun_def_node;
        StartNode start_node;
    };
} StmtNode;

// STATEMENT NODE


ParamNode* param_node_new(Char* mut, Char* name, Char* type);

ExprNode* number_node_new(Char* value);
ExprNode* var_ref_node_new(Char* name);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right);
ExprNode* unaryop_node_new(TokenType op, ExprNode* operand);
ExprNode* fun_call_node_new(const Char* name, List* expr_node_list);

InstrNode* var_def_node_new(Char* mut, const Char* type, const Char* name, ExprNode* value);
InstrNode* var_modif_node_new(const Char* name, ExprNode* value);
InstrNode* return_node_new(ExprNode* expr_node);
InstrNode* syscallwrite_node_new(Char* char_location_ptr_name);

StmtNode* fun_def_node_new(Char* name, Char* return_type, List* param_node_list, CodeblockNode* code_block);
StmtNode* start_node_new(CodeblockNode* code_block);

CodeblockNode* code_block_new(List* stmt_node_list);

Str* print_nodelist(const List* node_list);

#endif
