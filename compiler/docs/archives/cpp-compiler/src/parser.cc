#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "global.hh"


std::vector<Token> tokens;
size_t token_index = 0;
std::unordered_map<std::string, bool> variableInfos;

// define some functions 
NodePtr parse_prog();
NodePtr parse_stmt();
NodePtr parse_if();
std::vector<NodePtr> parse_block();
NodePtr parse_factor();
NodePtr parse_expr();
NodePtr parse_term();

Token consume(const TokenType expected_type = UNDEFINED)
{
    if (token_index < tokens.size()) {
        Token current_token = tokens[token_index++];
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
    if (tokens[token_index].type == NUMBER) {
        Token token = consume(NUMBER);
        return std::make_shared<NumberNode>(std::stoi(token.value));
    }
    else if (tokens[token_index].type == IDENTIFIER) {
        Token token = consume(IDENTIFIER);
        return std::make_shared<VarRefNode>(token.value);
    }
    else if (tokens[token_index].type == QUOTE) {
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

NodePtr parse_expression_with_priority(std::function<NodePtr()> parse_lower_priority, const std::vector<TokenType> &operators)
{
    NodePtr left = parse_lower_priority();
    while (token_index < tokens.size() && std::find(operators.begin(), operators.end(), tokens[token_index].type) != operators.end()) {
        Token token = consume();
        NodePtr right = parse_lower_priority();
        left = std::make_shared<BinOpNode>(token.line_number, left, token.type, right);
    }
    return left;
}

NodePtr parse_parentheses()
{
    if (tokens[token_index].type == LPAREN) {
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

NodePtr par_bool_op()
{
    return parse_expression_with_priority(parse_bin_or, {EQUALS_EQUALS, NOT_EQUALS, LESS_THAN, LESS_THAN_EQUALS, GREATER_THAN, GREATER_THAN_EQUALS});
}

NodePtr parse_expr()
{
    // handle input function
    if (tokens[token_index].value == "input") {
        int line_number = consume(IDENTIFIER).line_number;
        consume(LPAREN);
        consume(RPAREN);
        return std::make_shared<FunctionCallNode>(line_number, "input");
    }

    // binary expression
    return parse_expression_with_priority(par_bool_op, {AND, OR});
}

std::vector<NodePtr> parse_block()
{
    consume(LBRACE);
    std::vector<NodePtr> statements;
    while (token_index < tokens.size() && tokens[token_index].type != RBRACE) {
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
    if (tokens[token_index].type == ELSE) {
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
    if (tokens[token_index].type == IF) {
        return parse_if();
    }

    // handle while statement
    else if (tokens[token_index].type == WHILE) {
        return parse_while();
    }

    // handle variable declaration
    else if ((tokens[token_index].type == VAR || tokens[token_index].type == CST)) {

        var_decl_count++;
        
        // handling constant
        bool constant;
        if (tokens[token_index].type == VAR) {
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
        int line_number = consume(END_LINE).line_number;
        // add variable to the map
        variableInfos.insert({var_name, constant});

        return std::make_shared<VarDeclNode>(line_number, constant, type, var_name, value);
    }

    // handle variable modifiaction
    else if (tokens[token_index].type == IDENTIFIER && tokens[token_index + 1].type == EQUALS) {
        std::string var_name = consume(IDENTIFIER).value;

        // check if the variable is declared as cst
        auto varIt = variableInfos.find(var_name);
        if (varIt != variableInfos.end() && varIt->second) {
            display_and_throw_error("assignment of read-only variable '" + var_name + "'",
                                    tokens[token_index].line_number);
            exit(1);
        }

        int line_number = consume(EQUALS).line_number;
        NodePtr value = parse_expr();
        consume(END_LINE);
        return std::make_shared<VarModifNode>(line_number, var_name, value);
    }

    // handle print function
    else if (tokens[token_index].value == "print") {
        Token token = consume(IDENTIFIER);
        consume(LPAREN);
        std::vector<NodePtr> args;
        while (tokens[token_index].type != RPAREN) {
            args.push_back(parse_expr());
            if (tokens[token_index].type == COMMA) {
                consume(COMMA);
            }
        }
        consume(RPAREN);
        consume(END_LINE);
        return std::make_shared<FunctionCallNode>(token.line_number, token.value, args);
    }

    // handle exit function
    else if (tokens[token_index].value == "exit") {
        Token token = consume(IDENTIFIER);
        consume(LPAREN);
        consume(RPAREN);
        consume(END_LINE);
        return std::make_shared<FunctionCallNode>(token.line_number, "exit");
    }

    // else, there is an error
    display_and_throw_error("the provided code does not match any known statement pattern",
                            tokens[token_index].line_number);
    exit(1);
}

NodePtr parse_prog()
{
    std::vector<NodePtr> statements;
    while (token_index < tokens.size() && tokens[token_index].type != EOF_TOKEN) {
        statements.push_back(parse_stmt());
    }
    return std::make_shared<ProgramNode>(statements);
}

NodePtr parse(const std::vector<Token> &inputTokens)
{
    tokens = inputTokens;
    token_index = 0;
    return parse_prog();
}

std::string &print_ast(const NodePtr &node, std::string &output, const std::string &indent = "", bool last = true, bool is_value = false)
{
    // branch symbol management
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

    // indentation management
    std::string next_indent = indent;
    if (last) {
        next_indent += "   ";
    }
    else {
        next_indent += "│  ";
    }

    // checking the current node type
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
        output += next_indent + "├─ line_number: " + std::to_string(bnode->_line_number) + "\n";
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
        output += next_indent + "├─ line_number: " + std::to_string(vnode->_line_number) + "\n";
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
        output += next_indent + "├─ line_number: " + std::to_string(vnode->_line_number) + "\n";
        output += next_indent + "├─ name: " + vnode->_name + "\n";
        output += next_indent + "└─ value: " + "\n";
        if (vnode->_value) {
            print_ast(vnode->_value, output, next_indent + "   ", true, true);
        }
    }

    else if (FunctionCallNode *funccallnode = dynamic_cast<FunctionCallNode *>(node.get())) {
        output += indent + branch + "FunctionCall" + "\n";
        output += next_indent + "├─ line_number: " + std::to_string(funccallnode->_line_number) + "\n";
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