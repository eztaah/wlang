#ifndef NODE_H
#define NODE_H

#include "lib.h"
#include "token.h"

typedef struct {
    Char* name;
    Char* size;
} ParamNode;

// OTHER NODE

typedef struct ExprNode ExprNode;

typedef struct {
    Char* value;
} NumNode;

typedef struct {
    Char* name;
} VarrefNode;

typedef struct {
    ExprNode* expr;
} AddrderefNode;

typedef struct {
    Char* name;
} VaraddrNode;

typedef struct {
    TokenType op;
    ExprNode* operand;
} UnaropNode;

typedef struct {
    ExprNode* left;
    TokenType op;
    ExprNode* right;
} BinopNode;

typedef struct {
    Char* name;
    List* expr_node_list;
} FuncallNode;

typedef struct {
    List* expr_node_list;
} SyscNode;

typedef struct {
    
} ArrayNode;

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
} BlockNode;

typedef struct {
    Char* item_size;
    Char* name;
    Char* size;
    List* expr_node_list;
} ArraydecNode;

typedef struct {
    Char* size;
    Char* name;
    ExprNode* value;
} VardecNode;

typedef struct {
    ExprNode* cond_node;
    BlockNode* true_block;
    BlockNode* false_block;
} IfNode;

typedef struct {
    BlockNode* block;
} LoopNode;

typedef struct {
    Char* nothing;
} BreakNode;

typedef struct {
    ExprNode* lvalue;
    ExprNode* value;
} AssNode;

typedef struct {
    ExprNode* expr_node;
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
} FundefNode;

// DEFINITION NODES

ParamNode* param_node_new(Char* name, Char* size);

ExprNode* num_node_new(Char* value);
ExprNode* varref_node_new(Char* name);
ExprNode* addrderef_node_new(ExprNode* expr);
ExprNode* varaddr_node_new(Char* name);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right);
ExprNode* unarop_node_new(TokenType op, ExprNode* operand);
ExprNode* funcall_node_new(const Char* name, List* expr_node_list);
ExprNode* sysc_node_new(List* expr_node_list);

StmtNode* arraydec_node_new(Char* item_size, Char* name, Char* size, List* expr_node_list);
StmtNode* vardec_node_new(Char* size, Char* name, ExprNode* value);
StmtNode* if_node_new(ExprNode* cond_node, BlockNode* true_block, BlockNode* false_block);
StmtNode* loop_node_new(BlockNode* block);
StmtNode* break_node_new(Void);
StmtNode* ass_node_new(ExprNode* lvalue, ExprNode* value);
StmtNode* ret_node_new(ExprNode* expr_node);

FundefNode* fundef_node_new(Char* name, Char* return_size, Char* scope, List* param_node_list, BlockNode* block_node);

BlockNode* block_node_new(List* def_node_list);

Str* print_nodelist(const List* node_list);

#endif
