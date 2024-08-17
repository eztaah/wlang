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
        : _condition(condition), _true_block(true_bloc
static StmtNode* parse_stmt(Parser* parser)
{
    // parse return statement
    if (parser->current_token.type == TOKEN_RET) {
        StmtNode* ret_node = parse_ret(parser);
        eat_next_token(parser, TOKEN_SEMI);
        return ret_node;
    }

    // handle declarations
    else if (parser->current_token.type == TOKEN_LESSTHAN) {
        eat_next_token(parser, TOKEN_LESSTHAN);
        Token size_token = eat_next_token(parser, TOKEN_NUM);
        Char* size = strdup(size_token.value);
        eat_next_token(parser, TOKEN_GREATERTHAN);
    
        Token name_token = eat_next_token(parser, TOKEN_ID);
        Char* name = strdup(name_token.value);    

        // handle variable dec without assignement
        if (parser->current_token.type == TOKEN_SEMI) {
            ExprNode* expr_node = NULL;

            eat_next_token(parser, TOKEN_SEMI);   
            return vardec_node_new(size, name, expr_node);
        }

        // handle variable dec with assignement
        else if (parser->current_token.type == TOKEN_EQUAL) {
            eat_next_token(parser, TOKEN_EQUAL);
            ExprNode* expr_node = parse_expr(parser);

            eat_next_token(parser, TOKEN_SEMI);       
            return vardec_node_new(size, name, expr_node);
        }

        // handle array declarations (with and without assignement)
        else if (parser->current_token.type == TOKEN_LBRACKET) {
            eat_next_token(parser, TOKEN_LBRACKET);
            Token array_size_token = eat_next_token(parser, TOKEN_NUM);
            Char* array_size = strdup(array_size_token.value);
            eat_next_token(parser, TOKEN_RBRACKET);

            // with assignement
            if (parser->current_token.type == TOKEN_EQUAL) {
                eat_next_token(parser, TOKEN_EQUAL);

                // handling arguments
                eat_next_token(parser, TOKEN_LBRACKET);
                List* expr_node_list = list_new(sizeof(ExprNode));
                while (parser->current_token.type != TOKEN_RBRACKET) {
                    list_push(expr_node_list, parse_expr(parser));
                    if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last elt (and there is no comma after)
                        eat_next_token(parser, TOKEN_COMMA);
                    }
                }
                eat_next_token(parser, TOKEN_RBRACKET);

                eat_next_token(parser, TOKEN_SEMI);  
                return arraydec_node_new(size, name, array_size, expr_node_list);
            }

            // without assignement
            else if (parser->current_token.type == TOKEN_SEMI) {
                List* expr_node_list = NULL;

                eat_next_token(parser, TOKEN_SEMI);
                return arraydec_node_new(size, name, array_size, expr_node_list);
            }
            else {
                UNREACHABLE();
                return NULL;
            }
        }
    }

    // parse expression
    else {
        ExprNode* expr_node1 = parse_expr(parser);

        // handle var assignement
        if (parser->current_token.type == TOKEN_EQUAL) {
            eat_next_token(parser, TOKEN_EQUAL);

            // parse value of the var assigmeent
            ExprNode* expr_node2 = parse_expr(parser);

            eat_next_token(parser, TOKEN_SEMI);

            StmtNode* ass_node = ass_node_new(expr_node1, expr_node2);
            return ass_node;
        }

        else {
            StmtNode* stmt_node = expr_to_stmt_node(expr_node1);
            eat_next_token(parser, TOKEN_SEMI);

            return stmt_node;
        }
    }

    UNREACHABLE();
    return NULL;
}), _false_block(false_block) {}
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