#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <string>
#include <memory>
#include <vector>

typedef enum {
    CST,
    VAR,
    NUMBER,
    IDENTIFIER,
    EQUALS,

    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    BIN_AND,
    BIN_OR,
    XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,

    WHILE,
    IF,
    ELSE,

    SEMICOLON,
    COMMA,
    QUOTE,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    EQUALS_EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    LESS_THAN_EQUALS,
    GREATER_THAN,
    GREATER_THAN_EQUALS,

    UNDEFINED,
    EOF_TOKEN
} TokenType;

class Node {
  public:
    virtual ~Node() = default;
};

typedef std::shared_ptr<Node> NodePtr;

class NumberNode : public Node {
  public:
    NumberNode(int value) : _value(value) {}
    int _value;
};

class StringNode : public Node {
  public:
    StringNode(std::string content) : _content(content) {}
    std::string _content;
};

class VarRefNode : public Node {
  public:
    VarRefNode(const std::string &name) : _name(name) {}
    std::string _name;
};

class BinOpNode : public Node {
  public:
    BinOpNode(NodePtr left, const TokenType &op, NodePtr right)
        : _left(left), _op(op), _right(right) {}
    NodePtr _left;
    TokenType _op;
    NodePtr _right;
};

class ExpressionNode : public Node {
  public:
    ExpressionNode(NodePtr expression) : _expression(expression) {}
    NodePtr _expression;
};

class VarDeclNode : public Node { // VarDeclStatement
  public:
    VarDeclNode(TokenType type, const std::string &name, NodePtr value)
        : _type(type), _name(name), _value(value) {}
    TokenType _type;
    std::string _name;
    NodePtr _value;
};

class VarModifNode : public Node {
  public:
    VarModifNode(const std::string &name, NodePtr value)
        : _name(name), _value(value) {}
    std::string _name;
    NodePtr _value;
};

class IfNode : public Node { // IfStatement
  public:
    IfNode(NodePtr condition, std::vector<NodePtr> true_block, std::vector<NodePtr> false_block = {})
        : _condition(condition), _true_block(true_block), _false_block(false_block) {}
    NodePtr _condition;
    std::vector<NodePtr> _true_block;
    std::vector<NodePtr> _false_block;
};

class WhileNode : public Node {
  public:
    WhileNode(NodePtr condition, std::vector<NodePtr> block)
        : _condition(condition), _block(block) {}
    NodePtr _condition;
    std::vector<NodePtr> _block;
};

class ExpressionStatementNode : public Node {
  public:
    ExpressionStatementNode(NodePtr expression) : _expression(expression) {}
    NodePtr _expression;
};

class FunctionCallNode : public Node {
  public:
    FunctionCallNode(const std::string &name, const std::vector<NodePtr> &args = {})
        : _name(name), _args(args) {}
    std::string _name;
    std::vector<NodePtr> _args;
};

class ProgramNode : public Node {
  public:
    ProgramNode(const std::vector<NodePtr> &statements)
        : _statements(statements) {}
    std::vector<NodePtr> _statements;
};



typedef std::pair<TokenType, std::string> Token;

std::string token_to_string(TokenType type);

#endif // GLOBAL_HPP