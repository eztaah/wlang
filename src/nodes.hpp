#ifndef NODES_HPP
#define NODES_HPP

#include <memory>
#include <string>
#include <vector>

#include "lexer.hpp"

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

class IfNode : public Node { // IfStatement
  public:
    IfNode(NodePtr condition, std::vector<NodePtr> true_block,
           std::vector<NodePtr> false_block = {})
        : _condition(condition), _true_block(true_block),
          _false_block(false_block) {}
    NodePtr _condition;
    std::vector<NodePtr> _true_block;
    std::vector<NodePtr> _false_block;
};

class ExpressionStatementNode : public Node {
  public:
    ExpressionStatementNode(NodePtr expression) : _expression(expression) {}
    NodePtr _expression;
};

class FunctionCallNode : public Node {
  public:
    FunctionCallNode(const std::string &name, const std::vector<NodePtr> &args)
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

#endif // NODES_HPP
