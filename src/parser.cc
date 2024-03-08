#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "global.hh"

std::vector<Token> tokens;
size_t tokenIndex = 0;
std::unordered_map<std::string, bool> variableInfos;

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
        if (expected_type != UNDEFINED && current_token.type != expected_type) {
            display_and_throw_error("expected token type : \"" + token_to_string(expected_type) + "\", got (" + token_to_string(current_token.type) + ", \"" + current_token.value + "\")",
                                    current_token.line_number);
            exit(1);
        }
        return current_token;
    }
    // handle internal error
    display_and_throw_internal_error("attempted to consume a token, but no more tokens are available");
    exit(1);
}

NodePtr parse_term()
{
    if (tokens[tokenIndex].type == NUMBER) {
        Token token = consume(NUMBER);
        return std::make_shared<NumberNode>(std::stoi(token.value));
    }
    else if (tokens[tokenIndex].type == IDENTIFIER) {
        Token token = consume(IDENTIFIER);
        return std::make_shared<VarRefNode>(token.value);
    }
    else if (tokens[tokenIndex].type == QUOTE) {
        consume(QUOTE);
        Token token = consume(IDENTIFIER);
        NodePtr res = std::make_shared<StringNode>(token.value);
        consume(QUOTE);
        return res;
    }
    // handle errors
    display_and_throw_internal_error("internal parser error: return nullptr in parse_term()");
    exit(1);
}

NodePtr parse_expression_with_priority(std::function<NodePtr()> parseLowerPriority, const std::vector<TokenType> &operators)
{
    NodePtr left = parseLowerPriority();
    while (tokenIndex < tokens.size() && std::find(operators.begin(), operators.end(), tokens[tokenIndex].type) != operators.end()) {
        TokenType op = consume().type;
        NodePtr right = parseLowerPriority();
        left = std::make_shared<BinOpNode>(left, op, right);
    }
    return left;
}

NodePtr parse_parentheses()
{
    if (tokens[tokenIndex].type == LPAREN) {
        consume(LPAREN);
        NodePtr expr = parse_expr();
        consume(RPAREN);
        return expr;
    }
    return parse_term();
}

NodePtr parse_factor()
{
    return parse_expression_with_priority(parse_parentheses, {TIMES, DIVIDE, MODULO});
}

NodePtr parse_add()
{
    return parse_expression_with_priority(parse_factor, {PLUS, MINUS});
}

NodePtr parse_shift()
{
    return parse_expression_with_priority(parse_add, {SHIFT_RIGHT, SHIFT_LEFT});
}

NodePtr parse_bin_and()
{
    return parse_expression_with_priority(parse_shift, {BIN_AND});
}

NodePtr parse_xor()
{
    return parse_expression_with_priority(parse_bin_and, {XOR});
}

NodePtr parse_bin_or()
{
    return parse_expression_with_priority(parse_xor, {BIN_OR});
}

NodePtr parse_expr()
{
    // handle input function
    if (tokens[tokenIndex].value == "input") {
        // Input function
        consume(IDENTIFIER);
        consume(LPAREN);
        consume(RPAREN);
        return std::make_shared<FunctionCallNode>("input");
    }

    // Binary expression
    return parse_expression_with_priority(parse_bin_or, {EQUALS_EQUALS, NOT_EQUALS, LESS_THAN, LESS_THAN_EQUALS, GREATER_THAN, GREATER_THAN_EQUALS});
}

std::vector<NodePtr> parse_block()
{
    consume(LBRACE);
    std::vector<NodePtr> statements;
    while (tokenIndex < tokens.size() && tokens[tokenIndex].type != RBRACE) {
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
    if (tokens[tokenIndex].type == ELSE) {
        consume(ELSE);
        std::vector<NodePtr> false_branch = parse_block();
        return std::make_shared<IfNode>(condition, true_branch, false_branch);
    }
    else {
        return std::make_shared<IfNode>(condition, true_branch);
    }
}

NodePtr parse_while()
{
    consume(WHILE);
    consume(LPAREN);
    NodePtr condition = parse_expr();
    consume(RPAREN);
    std::vector<NodePtr> block = parse_block();
    return std::make_shared<WhileNode>(condition, block);
}

NodePtr parse_stmt()
{
    // handle if statements
    if (tokens[tokenIndex].type == IF) {
        return parse_if();
    }

    // handle while statement
    else if (tokens[tokenIndex].type == WHILE) {
        return parse_while();
    }

    // handle variable declaration
    else if ((tokens[tokenIndex].type == VAR || tokens[tokenIndex].type == CST)) {
        // handling constant
        bool constant;
        if (tokens[tokenIndex].type == VAR) {
            consume(VAR);
            constant = false;
        }
        else {
            consume(CST);
            constant = true;
        }
        // handling name
        std::string var_name = consume(IDENTIFIER).value;
        consume(COLON);
        // handling type
        std::string type = consume(TYPE).value;
        // handling content
        consume(EQUALS);
        NodePtr value = parse_expr();
        consume(SEMICOLON);
        // add variable to the map
        variableInfos.insert({var_name, constant});

        return std::make_shared<VarDeclNode>(constant, type, var_name, value);
    }
    // handle variable modifiaction
    else if (tokens[tokenIndex].type == IDENTIFIER && tokens[tokenIndex + 1].type == EQUALS) {
        std::string var_name = consume(IDENTIFIER).value;

        // check if the variable is declared as cst
        auto varIt = variableInfos.find(var_name);
        if (varIt != variableInfos.end() && varIt->second) {
            display_and_throw_error("assignment of read-only variable '" + var_name + "'",
                                    tokens[tokenIndex].line_number);
            exit(1);
        }

        consume(EQUALS);
        NodePtr value = parse_expr();
        consume(SEMICOLON);
        return std::make_shared<VarModifNode>(var_name, value);
    }

    // handle print function
    else if (tokens[tokenIndex].value == "print") {
        Token token = consume(IDENTIFIER);
        consume(LPAREN);
        std::vector<NodePtr> args;
        while (tokens[tokenIndex].type != RPAREN) {
            args.push_back(parse_expr());
            if (tokens[tokenIndex].type == COMMA) {
                consume(COMMA);
            }
        }
        consume(RPAREN);
        consume(SEMICOLON);
        return std::make_shared<FunctionCallNode>(token.value, args);
    }

    // Else, there is an error
    display_and_throw_error("the provided code does not match any known statement pattern",
                            tokens[tokenIndex].line_number);
    exit(1);
}

NodePtr parse_prog()
{
    std::vector<NodePtr> statements;
    while (tokenIndex < tokens.size() && tokens[tokenIndex].type != EOF_TOKEN) {
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
            print_ast(pnode->_statements[i], output, next_indent, i == pnode->_statements.size() - 1);
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
    else if (WhileNode *wnode = dynamic_cast<WhileNode *>(node.get())) {
        output += indent + branch + "WhileNode" + "\n";
        output += next_indent + "├─ condition: " + "\n";
        print_ast(wnode->_condition, output, next_indent + "│  ", true);
        if (!wnode->_block.empty()) {
            output += next_indent + "└─ _block: " + "\n";
            for (size_t i = 0; i < wnode->_block.size(); ++i) {
                print_ast(wnode->_block[i], output, next_indent + "   ", i == wnode->_block.size() - 1);
            }
        }
        else {
            output += next_indent + "└── _block: " + "\n";
        }
    }

    else if (BinOpNode *bnode = dynamic_cast<BinOpNode *>(node.get())) {
        output += indent + branch + "BinOpNode" + "\n";
        output += next_indent + "├─ op: " + token_to_string(bnode->_op) + "\n";
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
        output += next_indent + "├─ constant: " + std::to_string(vnode->_constant) + "\n";
        output += next_indent + "├─ name: " + vnode->_name + "\n";
        output += next_indent + "├─ type: " + vnode->_type + "\n";
        output += next_indent + "└─ value: " + "\n";
        if (vnode->_value) {
            print_ast(vnode->_value, output, next_indent + "   ", true, true);
        }
    }
    else if (VarModifNode *vnode = dynamic_cast<VarModifNode *>(node.get())) {
        output += indent + branch + "VarModifNode" + "\n";
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
    else {
        display_and_throw_internal_error("unknow node encontered while displaying the ast");
        exit(1);
    }

    return output;
}