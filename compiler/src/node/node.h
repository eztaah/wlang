#ifndef NODE_H
#define NODE_H

#include "lib.h"
#include "token.h"

typedef struct {
    char* name;
    char* size;
    int line;
} ParamNode;

// OTHER NODE

typedef struct ExprNode ExprNode;

typedef struct {
    char* value;
    int line;
} NumNode;

typedef struct {
    char* name;
    int line;
} VarrefNode;

typedef struct {
    char* name;
    int line;
} VaraddrNode;

typedef struct {
    TokenType op;
    ExprNode* operand;
    int line;
} UnaropNode;

typedef struct {
    ExprNode* left;
    TokenType op;
    ExprNode* right;
    int line;
} BinopNode;

typedef struct {
    char* name;
    List* expr_node_list;
    int line;
} FuncallNode;

typedef struct {
    List* expr_node_list;
    int line;
} SyscNode;

struct ExprNode {
    enum {
        NODE_NUM = 10,
        NODE_VARREF = 11,
        NODE_VARADDR = 13,
        NODE_BINOP = 14,
        NODE_UNARYOP = 15,
        NODE_FUNCALL = 16,
        NODE_SYSC = 17,
    } type;
    int line;
    union {
        BinopNode binop_node;
        UnaropNode unarop_node;
        VarrefNode varref_node;
        VaraddrNode varaddr_node;
        NumNode num_node;
        FuncallNode funcall_node;
        SyscNode sysc_node;
    };
};

// EXPRESSION NODES

typedef struct {
    List* stmt_node_list;
    int line;
} BlockNode;

typedef struct {
    char* item_size;
    char* name;
    char* size;
    List* expr_node_list;
    int line;
} ArraydecNode;

typedef struct {
    char* size;
    char* name;
    ExprNode* value;
    int line;
} VardecNode;

typedef struct {
    ExprNode* cond_node;
    BlockNode* true_block;
    BlockNode* false_block;
    int line;
} IfNode;

typedef struct {
    BlockNode* block;
    int line;
} LoopNode;

typedef struct {
    int line;
} BreakNode;

typedef struct {
    ExprNode* lvalue;
    ExprNode* value;
    int line;
} AssNode;

typedef struct {
    ExprNode* expr_node;
    int line;
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
    int line;
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
    char* name;
    char* return_size;
    char* scope;
    List* param_node_list;
    BlockNode* block_node;
    int line;
} FundefNode;

// DEFINITION NODES

ParamNode* param_node_new(char* name, char* size, int line);

ExprNode* num_node_new(char* value, int line);
ExprNode* varref_node_new(char* name, int line);
ExprNode* varaddr_node_new(char* name, int line);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right, int line);
ExprNode* unarop_node_new(TokenType op, ExprNode* operand, int line);
ExprNode* funcall_node_new(const char* name, List* expr_node_list, int line);
ExprNode* sysc_node_new(List* expr_node_list, int line);

StmtNode* arraydec_node_new(char* item_size, char* name, char* size, List* expr_node_list, int line);
StmtNode* vardec_node_new(char* size, char* name, ExprNode* value, int line);
StmtNode* if_node_new(ExprNode* cond_node, BlockNode* true_block, BlockNode* false_block, int line);
StmtNode* loop_node_new(BlockNode* block, int line);
StmtNode* break_node_new(int line);
StmtNode* ass_node_new(ExprNode* lvalue, ExprNode* value, int line);
StmtNode* ret_node_new(ExprNode* expr_node, int line);

FundefNode* fundef_node_new(char* name, char* return_size, char* scope, List* param_node_list, BlockNode* block_node, int line);

BlockNode* block_node_new(List* def_node_list, int line);

Str* print_nodelist(const List* node_list);

#endif
