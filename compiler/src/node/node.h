#ifndef NODE_H
#define NODE_H

#include "lib.h"
#include "token.h"

typedef struct {
    Char* name;
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
        BinopNode binop_node;
        UnaropNode unarop_node;
        VarrefNode varref_node;
        VaraddrNode varaddr_node;
        NumNode num_node;
        FuncallNode funcall_node;
    };
};

// EXPRESSION NODES

typedef struct {
    Char* name;
    ExprNode* value;
} VarAssNode;

typedef struct {
    Bool is_empty; // not the right way of doing it but ok for now
    ExprNode* expr_node;
} RetNode;

typedef struct {
    List* expr_node_list;
} SyscNode;

typedef struct {
    enum {
        NODE_VARASS,
        NODE_RET,
        NODE_SYSC,
        NODE_EXPR
    } type;
    union {
        VarAssNode varass_node;
        RetNode ret_node;
        SyscNode sysc_node;
        ExprNode expr_node;
    };
} StmtNode;

// STATEMENT NODES

typedef struct {
    List* stmt_node_list;
} CodeblockNode;

typedef struct {
    Char* name;
    List* param_node_list;
    CodeblockNode* codeblock_node;
} FundefNode;

// DEFINITION NODES

ParamNode* param_node_new(Char* name);

ExprNode* num_node_new(Char* value);
ExprNode* varref_node_new(Char* name);
ExprNode* varaddr_node_new(Char* name);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right);
ExprNode* unarop_node_new(TokenType op, ExprNode* operand);
ExprNode* funcall_node_new(const Char* name, List* expr_node_list);

StmtNode* varass_node_new(const Char* name, ExprNode* value);
StmtNode* ret_node_new(Bool is_empty, ExprNode* expr_node);
StmtNode* sysc_node_new(List* expr_node_list);

FundefNode* fundef_node_new(Char* name, List* param_node_list, CodeblockNode* codeblock_node);

CodeblockNode* codeblock_node_new(List* def_node_list);

Str* print_nodelist(const List* node_list);

#endif
