#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "nodes.hpp"

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

Token consume(const TokenType expected_type = UNDEFINED)
{
    if (tokenIndex < tokens.size()) {
        Token current_token = tokens[tokenIndex++];
        if (expected_type != UNDEFINED && current_token.first != expected_type) {
            throw std::runtime_error("Expected token type " + tokenTypeToString(expected_type) + ", got (" + tokenTypeToString(current_token.first) + ", " + current_token.second + ")");
        }
        return current_token;
    }
    throw std::runtime_error("No more tokens");
}

NodePtr parse_term()
{
    if (tokens[tokenIndex].first == NUMBER) {
        Token token = consume(NUMBER);
        return std::make_shared<NumberNode>(std::stoi(token.second));
    }
    if (tokens[tokenIndex].first == QUOTE) {
        consume(QUOTE);
        Token token = consume(IDENTIFIER);
        NodePtr res = std::make_shared<StringNode>(token.second);
        consume(QUOTE);
        return res;
    }
    if (tokens[tokenIndex].first == IDENTIFIER) {
        Token token = consume(IDENTIFIER);
        return std::make_shared<VarRefNode>(token.second);
    }
    return nullptr;
}

NodePtr parse_factor()
{
    NodePtr node = parse_term();
    while (tokenIndex < tokens.size() && (tokens[tokenIndex].first == TIMES || tokens[tokenIndex].first == DIVIDE)) {
        TokenType op = consume().first;
        NodePtr right = parse_term();
        node = std::make_shared<BinOpNode>(node, op, right);
    }
    return node;
}

NodePtr parse_expr()
{
    Token currentToken = tokens[tokenIndex];
    Token nextToken = tokens[tokenIndex + 1];

    NodePtr left = parse_factor();

    // Binary expression
    while (tokenIndex < tokens.size() && (tokens[tokenIndex].first == PLUS || tokens[tokenIndex].first == MINUS || tokens[tokenIndex].first == EQUALS_EQUALS)) {
        TokenType op = consume().first;
        NodePtr right = parse_factor();
        left = std::make_shared<BinOpNode>(left, op, right);
    }
    return left;
}

std::vector<NodePtr> parse_block()
{
    consume(LBRACE);
    std::vector<NodePtr> statements;
    while (tokenIndex < tokens.size() && tokens[tokenIndex].first != RBRACE) {
        statements.push_back(parse_stmt());
    }
    consume(RBRACE);
    return statements;
}

NodePtr parse_if()
{
    consume(IF);
    consume(LPAREN);
    NodePtr condition = parse_expr();
    consume(RPAREN);
    std::vector<NodePtr> true_branch = parse_block();
    if (tokens[tokenIndex].first == ELSE) {
        consume(ELSE);
        std::vector<NodePtr> false_branch = parse_block();
        return std::make_shared<IfNode>(condition, true_branch, false_branch);
    }
    else {
        return std::make_shared<IfNode>(condition, true_branch);
    }
}

NodePtr parse_stmt()
{
    // handle if statements
    if (tokens[tokenIndex].first == IF) {
        return parse_if();
    }
    // handle variable declaration
    else if ((tokens[tokenIndex].first == VAR || tokens[tokenIndex].first == LET) && tokens[tokenIndex + 1].first == IDENTIFIER && tokens[tokenIndex + 2].first == EQUALS) {
        TokenType type = consume().first;
        std::string var_name = consume(IDENTIFIER).second;
        consume(EQUALS);
        NodePtr value = parse_expr();
        consume(SEMICOLON);
        return std::make_shared<VarDeclNode>(type, var_name, value);
    }

    // Println
    else if (tokens[tokenIndex].first == IDENTIFIER) {
        if (tokens[tokenIndex].second == "println") {
            Token left = consume(IDENTIFIER);
            consume(LPAREN);
            std::vector<NodePtr> args;
            while (tokens[tokenIndex].first != RPAREN) {
                args.push_back(parse_expr());
                if (tokens[tokenIndex].first == COMMA) {
                    consume(COMMA);
                }
            }
            consume(RPAREN);
            consume(SEMICOLON);
            return std::make_shared<FunctionCallNode>(left.second, args);
        }
        else {
            std::cout << "\033[31m[!] Compilation error : Fonction non autorisée appelée: " + tokens[tokenIndex].second + "\033[0m" << std::endl;
            exit(-1);
        }
    }

    return nullptr;
}

NodePtr parse_prog()
{
    std::vector<NodePtr> statements;
    while (tokenIndex < tokens.size() && tokens[tokenIndex].first != EOF_TOKEN) {
        statements.push_back(parse_stmt());
    }
    return std::make_shared<ProgramNode>(statements);
}

NodePtr parse(const std::vector<Token> &inputTokens)
{
    tokens = inputTokens;
    tokenIndex = 0;
    return parse_prog();
}

std::string &print_ast(const NodePtr &node, std::string &output, const std::string &indent = "", bool last = true, bool is_value = false)
{
    // Branch symbol management
    std::string branch;
    if (is_value) {
        branch = "";
    }
    else {
        if (last) {
            branch = "└─ ";
        }
        else {
            branch = "├─ ";
        }
    }

    // Indentation management
    std::string next_indent = indent;
    if (last) {
        next_indent += "   ";
    }
    else {
        next_indent += "│  ";
    }

    // Checking the current node type
    if (ProgramNode *pnode = dynamic_cast<ProgramNode *>(node.get())) {
        output += indent + "ProgramNode" + "\n";
        for (size_t i = 0; i < pnode->_statements.size(); ++i) {
            print_ast(pnode->_statements[i], output, next_indent,
                      i == pnode->_statements.size() - 1);
        }
    }

    else if (IfNode *inode = dynamic_cast<IfNode *>(node.get())) {
        output += indent + branch + "IfNode" + "\n";
        output += next_indent + "├─ condition: " + "\n";
        print_ast(inode->_condition, output, next_indent + "│  ", true);
        if (!inode->_true_block.empty()) {
            output += next_indent + "└─ true_block: " + "\n";
            for (size_t i = 0; i < inode->_true_block.size(); ++i) {
                print_ast(inode->_true_block[i], output, next_indent + "   ", i == inode->_true_block.size() - 1);
            }
        }
        else {
            output += next_indent + "└── true_block: " + "\n";
        }
        if (!inode->_false_block.empty()) {
            output += next_indent + "└─ false_block: " + "\n";
            for (size_t i = 0; i < inode->_false_block.size(); ++i) {
                print_ast(inode->_false_block[i], output, next_indent + "   ", i == inode->_false_block.size() - 1);
            }
        }
    }

    else if (BinOpNode *bnode = dynamic_cast<BinOpNode *>(node.get())) {
        output += indent + branch + "BinOpNode" + "\n";
        output += next_indent + "├─ op: " + tokenTypeToString(bnode->_op) + "\n";
        output += next_indent + "├─ left: " + "\n";
        print_ast(bnode->_left, output, next_indent + "│  ", false);
        output += next_indent + "└─ right: " + "\n";
        print_ast(bnode->_right, output, next_indent + "   ");
    }

    else if (NumberNode *nnode = dynamic_cast<NumberNode *>(node.get())) {
        output += indent + branch + "NumberNode(value=" + std::to_string(nnode->_value) + ")" + "\n";
    }
    else if (StringNode *nnode = dynamic_cast<StringNode *>(node.get())) {
        output += indent + branch + "StringNode(value=" + nnode->_content + ")" + "\n";
    }

    else if (VarDeclNode *vnode = dynamic_cast<VarDeclNode *>(node.get())) {
        output += indent + branch + "VarDeclNode" + "\n";
        output +=
            next_indent + "├─ type: " + tokenTypeToString(vnode->_type) + "\n";
        output += next_indent + "├─ name: " + vnode->_name + "\n";
        output += next_indent + "└─ value: " + "\n";
        if (vnode->_value) {
            print_ast(vnode->_value, output, next_indent + "   ", true, true);
        }
    }

    else if (FunctionCallNode *funccallnode = dynamic_cast<FunctionCallNode *>(node.get())) {
        output += indent + branch + "FunctionCall" + "\n";
        output += next_indent + "├─ name: " + funccallnode->_name + "\n";
        output += next_indent + "├─ arguments: " + "\n";
        for (NodePtr arg : funccallnode->_args) {
            print_ast(arg, output, next_indent + "│  ", false);
        }
    }

    else if (VarRefNode *vrefnode = dynamic_cast<VarRefNode *>(node.get())) {
        output +=
            indent + branch + "VarRefNode(name=" + vrefnode->_name + ")" + "\n";
    }

    return output;
}