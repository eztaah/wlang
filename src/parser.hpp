// parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include <string>
#include "nodes.hpp"
#include "lexer.hpp"

NodePtr parse(const std::vector<Token>& tokens);
void print_ast(const NodePtr& node, const std::string& indent = "", bool last = true, bool is_value = false);

#endif // PARSER_HPP
