#ifndef GLOBAL_HH
#define GLOBAL_HH

#include <memory>
#include <string>
#include <vector>


// define flags
extern bool dev_mode;
extern bool to_executable;
extern bool runtime_error_flag;
extern std::string output_directory;
extern int var_decl_count;

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
    MODULO,
    BIN_AND,
    BIN_OR,
    XOR,
    SHIFT_LEFT,
    SHIFT_RIGHT,

    WHILE,
    IF,
    ELSE,

    END_LINE,
    COMMA,
    QUOTE,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COLON,
    TYPE,

    EQUALS_EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    LESS_THAN_EQUALS,
    GREATER_THAN,
    GREATER_THAN_EQUALS,
    AND,
    OR,

    UNDEFINED,
    EOF_TOKEN
} TokenType;

std::string token_to_string(TokenType type);

void display_and_throw_internal_error(std::string error_message);
void display_and_throw_error(std::string error_message, int line_number = -1);

struct Token {
    int line_number;
    TokenType type;
    std::string value;

    Token(int line_number, TokenType type, std::string value) : line_number(line_number), type(type), value(value) {}
};

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
    BinOpNode(int line_number, NodePtr left, const TokenType &op, NodePtr right)
        : _line_number(line_number), _left(left), _op(op), _right(right) {}
    int _line_number;
    NodePtr _left;
    TokenType _op;
    NodePtr _right;
};

class VarDeclNode : public Node {
    public:
    VarDeclNode(int line_number, bool constant, std::string type, const std::string &name, NodePtr value)
        : _line_number(line_number), _constant(constant), _type(type), _name(name), _value(value) {}
    int _line_number;
    bool _constant;
    std::string _type;
    std::string _name;
    NodePtr _value;
};

class VarModifNode : public Node {
    public:
    VarModifNode(int line_number, const std::string &name, NodePtr value)
        : _line_number(line_number), _name(name), _value(value) {}
    int _line_number;
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

class WhileNode : public Node {
    public:
    WhileNode(NodePtr condition, std::vector<NodePtr> block)
        : _condition(condition), _block(block) {}
    NodePtr _condition;
    std::vector<NodePtr> _block;
};

class FunctionCallNode : public Node {
    public:
    FunctionCallNode(int line_number, const std::string &name, const std::vector<NodePtr> &args = {})
        : _line_number(line_number), _name(name), _args(args) {}
    int _line_number;
    std::string _name;
    std::vector<NodePtr> _args;
};

class ProgramNode : public Node {
    public:
    ProgramNode(const std::vector<NodePtr> &statements)
        : _statements(statements) {}
    std::vector<NodePtr> _statements;
};

#endif // GLOBAL_HH