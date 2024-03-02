#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

#include "nodes.hpp"
#include "lexer.hpp"


std::vector<Token> tokens;
size_t tokenIndex = 0;

// Ensures all functions are defined before execution
NodePtr parse_prog();
NodePtr parse_stmt();
NodePtr parse_if();
std::vector<NodePtr> parse_block();
NodePtr parse_factor();
NodePtr parse_expr();
NodePtr parse_term();


Token consume(const TokenType expected_type = UNDEFINED) {
    if (tokenIndex < tokens.size()) {
        Token current_token = tokens[tokenIndex++];
        if (expected_type != UNDEFINED  && current_token.first != expected_type) {
            throw std::runtime_error("Expected token type " + tokenTypeToString(expected_type) + ", got (" + tokenTypeToString(current_token.first) + ", " + current_token.second + ")");
        }
        return current_token;
    }
    throw std::runtime_error("No more tokens");
}

NodePtr parse_term() {
    if (tokens[tokenIndex].first == NUMBER) {
        Token token = consume(NUMBER);
        return std::make_shared<NumberNode>(std::stoi(token.second));
    }
    if (tokens[tokenIndex].first == IDENTIFIER) {
        Token token = consume(IDENTIFIER);
        return std::make_shared<VarRefNode>(token.second);
    }
    return nullptr;
}

NodePtr parse_factor() {
    NodePtr node = parse_term();
    while (tokenIndex < tokens.size() && (tokens[tokenIndex].first == TIMES || tokens[tokenIndex].first == DIVIDE)) {
        TokenType op = consume().first;
        NodePtr right = parse_term();
        node = std::make_shared<BinOpNode>(node, op, right);
    }
    return node;
}

NodePtr parse_expr() {
    NodePtr node = parse_factor();
    while (tokenIndex < tokens.size() && (tokens[tokenIndex].first == PLUS || tokens[tokenIndex].first == MINUS || tokens[tokenIndex].first == EQUALS_EQUALS)) {
        TokenType op = consume().first;
        NodePtr right = parse_factor();
        node = std::make_shared<BinOpNode>(node, op, right);
    }
    return node;
}

std::vector<NodePtr> parse_block() {
    consume(LBRACE);
    std::vector<NodePtr> statements;
    while (tokenIndex < tokens.size() && tokens[tokenIndex].first != RBRACE) {
        statements.push_back(parse_stmt());
    }
    consume(RBRACE);
    return statements;
}

NodePtr parse_if() {
    consume(IF);
    consume(LPAREN);
    NodePtr condition = parse_expr();
    consume(RPAREN);
    std::vector<NodePtr> true_branch = parse_block();
    // No support for 'else' at the moment
    return std::make_shared<IfNode>(condition, true_branch);
}

NodePtr parse_stmt() {
    if (tokens[tokenIndex].first == IF) {
        return parse_if();
    }
    if (tokens[tokenIndex].first == IDENTIFIER && tokens[tokenIndex + 1].first == EQUALS) {
        std::string var_name = consume(IDENTIFIER).second;
        consume(EQUALS);
        NodePtr value = parse_expr();
        consume(SEMICOLON);
        return std::make_shared<VarDeclNode>(var_name, value);
    }
    return nullptr;
}

NodePtr parse_prog() {
    std::vector<NodePtr> statements;
    while (tokenIndex < tokens.size() && tokens[tokenIndex].first != EOF_TOKEN) {
        statements.push_back(parse_stmt());
    }
    return std::make_shared<ProgramNode>(statements);
}

NodePtr parse(const std::vector<Token>& inputTokens) {
    tokens = inputTokens;
    tokenIndex = 0;
    return parse_prog();
}

void print_ast(const NodePtr& node, const std::string& indent = "", bool last = true, bool is_value = false) {
    // Branch symbol management
    std::string branch;
    if (is_value) {
        branch = "";
    } else {
        if (last) {
            branch = "└─ ";
        } else {
            branch = "├─ ";
        }
    }

    // Indentation management
    std::string next_indent = indent;
    if (last) {
        next_indent += "   ";
    } else {
        next_indent += "│  ";
    }
        
    // Checking the current node type
    if (ProgramNode* pnode = dynamic_cast<ProgramNode*>(node.get())) {
        std::cout << indent << "ProgramNode" << std::endl;
        for (size_t i = 0; i < pnode->_statements.size(); ++i) {
            print_ast(pnode->_statements[i], next_indent, i == pnode->_statements.size() - 1);
        }
    } 
    else if (IfNode* inode = dynamic_cast<IfNode*>(node.get())) {
        std::cout << indent << branch << "IfNode" << std::endl;
        std::cout << next_indent << "├─ condition: " << std::endl;
        print_ast(inode->_condition, next_indent + "│  ", true);
        if (!inode->_true_block.empty()) {
            std::cout << next_indent << "└─ true_block: " << std::endl;
            for (size_t i = 0; i < inode->_true_block.size(); ++i) {
                print_ast(inode->_true_block[i], next_indent + "   ", i == inode->_true_block.size() - 1);
            }
        } else {
            std::cout << next_indent << "└── true_block: " << std::endl;
        }
    } 
    else if (BinOpNode* bnode = dynamic_cast<BinOpNode*>(node.get())) {
        std::cout << indent << branch << "BinOpNode" << std::endl;
        std::cout << next_indent << "├─ op: " << tokenTypeToString(bnode->_op) << std::endl;
        std::cout << next_indent << "├─ left: " << std::endl;
        print_ast(bnode->_left, next_indent + "│  ", false);
        std::cout << next_indent << "└─ right: " << std::endl;
        print_ast(bnode->_right, next_indent + "   ");
    } 
    else if (NumberNode* nnode = dynamic_cast<NumberNode*>(node.get())) {
        std::cout << indent << branch << "NumberNode(value=" << nnode->_value << ")" << std::endl;
    } 
    else if (VarDeclNode* vnode = dynamic_cast<VarDeclNode*>(node.get())) {
        std::cout << indent << branch << "VarDeclNode" << std::endl;
        std::cout << next_indent << "├─ name: " << vnode->_name << std::endl;
        std::cout << next_indent << "└─ value: " << std::endl;
        if (vnode->_value) {
            print_ast(vnode->_value, next_indent + "   ", true, true);
        }
    } 
    else if (VarRefNode* vrefnode = dynamic_cast<VarRefNode*>(node.get())) {
        std::cout << indent << branch << "VarRefNode(name=" << vrefnode->_name << ")" << std::endl;
    }
}