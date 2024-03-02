#ifndef NODES_HPP
#define NODES_HPP

#include <vector>
#include <memory>
#include <string>

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
    VarRefNode(const std::string& name) : _name(name) {}
    std::string _name;
};

class BinOpNode : public Node {
public:
    BinOpNode(NodePtr left, const std::string& op, NodePtr right)
        : _left(left), _op(op), _right(right) {}
    NodePtr _left;
    std::string _op;
    NodePtr _right;
};

class VarDeclNode : public Node {
public:
    VarDeclNode(const std::string& name, NodePtr value)
        : _name(name), _value(value) {}
    std::string _name;
    NodePtr _value;
};

class IfNode : public Node {
public:
    IfNode(NodePtr condition, std::vector<NodePtr> true_block, std::vector<NodePtr> false_block = {})
        : _condition(condition), _true_block(true_block), _false_block(false_block) {}
    NodePtr _condition;
    std::vector<NodePtr> _true_block;
    std::vector<NodePtr> _false_block;
};

class ProgramNode : public Node {
public:
    ProgramNode(const std::vector<NodePtr>& statements) : _statements(statements) {}
    std::vector<NodePtr> _statements;
};

#endif // NODES_HPP
