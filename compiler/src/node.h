#ifndef NODE_H
#define NODE_H

#include "global.h"
#include "token.h"

typedef struct ExpressionNode ExpressionNode;

typedef struct {
    Char* value;
} NumberNode;

typedef struct {
    ExpressionNode* left;
    TokenType op;
    ExpressionNode* right;
} BinOpNode;

struct ExpressionNode {
    enum {
        NODE_NUMBER = 10,
        NODE_BINOP,
    } type;
    union {
        NumberNode number_node;
        BinOpNode bin_op_node;
    };
};

typedef struct {
    Char* type;
    Char* name;
    ExpressionNode* value;
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
} StatementNode;

ExpressionNode* instanciate_number_node(Char* value);
ExpressionNode* instanciate_binop_node(ExpressionNode* left, TokenType op, ExpressionNode* right);
StatementNode* instanciate_var_decl_node(const Char* type, const Char* name, ExpressionNode* value);

#endif
