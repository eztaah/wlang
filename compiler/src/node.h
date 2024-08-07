#ifndef NODE_H
#define NODE_H

#include "lib.h"
#include "token.h"

typedef struct ExprNode ExprNode;

typedef struct {
    Char* value;
} NumberNode;

typedef struct {
    TokenType op;   
    ExprNode* operand; 
} UnaryOpNode;

typedef struct {
    ExprNode* left;
    TokenType op;
    ExprNode* right;
} BinopNode;

struct ExprNode {
    enum {
        NODE_NUMBER = 10,
        NODE_BINOP = 11,
        NODE_UNARYOP = 12
    } type;
    union {
        BinopNode bin_op_node;
        UnaryOpNode unary_op_node;
        NumberNode number_node;
    };
};

typedef struct {
    Char* type;
    Char* name;
    ExprNode* value;
} VarDeclNode;

typedef struct {
    enum {
        NODE_VAR_DECL,
        // NODE_VAR_MODIF,
        // NODE_IF,
        // NODE_WHILE,
        // NODE_FUNC_CALL,
    } type;
    union {
        VarDeclNode var_decl;
        // VarModifNode var_modif;
        // IfNode if_node;
        // WhileNode while_node;
        // FunctionCallNode func_call;
    };
} StmtNode;

ExprNode* number_node_new(Char* value);
ExprNode* binop_node_new(ExprNode* left, TokenType op, ExprNode* right);
ExprNode* unaryop_node_new(TokenType op, ExprNode* operand);
StmtNode* var_decl_node_new(const Char* type, const Char* name, ExprNode* value);

#endif