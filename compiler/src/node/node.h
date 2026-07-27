#ifndef NODE_H
#define NODE_H

#include "lib.h"
#include "token.h"

typedef struct {
    Char* name;
    Char* size;
    I32 line_number;
} ParamNode;

// OTHER NODE

typedef struct ExprNode ExprNode;

typedef struct {
    Char* value;
    I32 line_number;
} NumNode;

typedef struct {
    Char* name;
    I32 line_number;
} VarrefNode;

typedef struct {
    ExprNode* expr;
    I32 line_number;
} AddrderefNode;

typedef struct {
    Char* name;
    I32 line_number;
} VaraddrNode;

typedef struct {
    TokenType op;
    ExprNode* operand;
    I32 line_number;
} UnaropNode;

typedef struct {
    ExprNode* left;
    TokenType op;
    ExprNode* right;
    I32 line_number;
} BinopNode;

typedef struct {
    Char* name;
    List* expr_node_list;
    I32 line_number;
} FuncallNode;

typedef struct {
    List* expr_node_list;
    I32 line_number;
} SyscNode;

struct ExprNode {
    enum {
        NODE_NUM = 10,
        NODE_VARREF = 11,
        NODE_ADDRDEREF = 12,
        NODE_VARADDR = 13,
        NODE_BINOP = 14,
        NODE_UNARYOP = 15,
        NODE_FUNCALL = 16,
        NODE_SYSC = 17,
    } type;
    I32 line_number;
    union {
        BinopNode binop_node;
        UnaropNode unarop_node;
        VarrefNode varref_node;
        AddrderefNode addrderef_node;
        VaraddrNode varaddr_node;
        NumNode num_node;
        FuncallNode funcall_node;
        SyscNode sysc_node;
    };
};

// EXPRESSION NODES

typedef struct {
    List* stmt_node_list;
    I32 line_number;
} BlockNode;

typedef struct {
    Char* item_size;
    Char* name;
    Char* size;
    List* expr_node_list;
    I32 line_number;
} ArraydecNode;

typedef struct {
    Char* size;
    Char* name;
    ExprNode* value;
    I32 line_number;
} VardecNode;

typedef struct {
    ExprNode* cond_node;
    BlockNode* true_block;
    BlockNode* false_block;
    I32 line_number;
} IfNode;

typedef struct {
    BlockNode* block;
    I32 line_number;
} LoopNode;

typedef struct {
    Char* nothing; // to remove
    I32 line_number;
} BreakNode;

typedef struct {
    ExprNode* lvalue;
    ExprNode* value;
    I32 line_number;
} AssNode;

typedef struct {
    ExprNode* expr_node;
    I32 line_number;
} RetNode;

typedef struct {
    enum {
        NODE_ARRAYDEC,
        NODE_VARDEC,
        NODE_IF,
        NODE_LOOP,
        NODE_BREAK,
        NODE_ASS,
        NODE_RET,
        NODE_EXPR,
    } type;
    I32 line_number;
    union {
        ArraydecNode arraydec_node;
        VardecNode vardec_node;
        IfNode if_node;
        LoopNode loop_node;
        BreakNode break_node;
        AssNode ass_node;
        RetNode ret_node;
        ExprNode expr_node;
    };
} StmtNode;

// STATEMENT NODES

typedef struct {
    Char* name;
    Char* return_size;
    Char* scope;
    List* param_node_list;
    BlockNode* block_node;
    I32 line_number;
} FundefNode;

// DEFINITION NODES

ParamNode* param_node_new(Char* name, Char* size, I32 line_number);

ExprNode* num_node_new(Char* value, I32 line_number);
ExprNode* varref_node_new(Char* name, I32 line_number);
ExprNode* addrderef_node_new(ExprNode* expr, I32 line_number);
ExprNode* varaddr_node_new(Char* name, I32 line_number);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right, I32 line_number);
ExprNode* unarop_node_new(TokenType op, ExprNode* operand, I32 line_number);
ExprNode* funcall_node_new(const Char* name, List* expr_node_list, I32 line_number);
ExprNode* sysc_node_new(List* expr_node_list, I32 line_number);

StmtNode* arraydec_node_new(Char* item_size, Char* name, Char* size, List* expr_node_list, I32 line_number);
StmtNode* vardec_node_new(Char* size, Char* name, ExprNode* value, I32 line_number);
StmtNode* if_node_new(ExprNode* cond_node, BlockNode* true_block, BlockNode* false_block, I32 line_number);
StmtNode* loop_node_new(BlockNode* block, I32 line_number);
StmtNode* break_node_new(I32 line_number);
StmtNode* ass_node_new(ExprNode* lvalue, ExprNode* value, I32 line_number);
StmtNode* ret_node_new(ExprNode* expr_node, I32 line_number);

FundefNode* fundef_node_new(Char* name, Char* return_size, Char* scope, List* param_node_list, BlockNode* block_node, I32 line_number);

BlockNode* block_node_new(List* def_node_list, I32 line_number);

Str* print_nodelist(const List* node_list);

#endif
