// parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "nodes.hpp"

NodePtr parse(const std::vector<Token> &tokens);
std::string &print_ast(const NodePtr &node, std::string &output, const std::string &indent = "", bool last = true, bool is_value = false);

#endif // PARSER_HPP
