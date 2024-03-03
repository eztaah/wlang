// parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include <string>
#include "nodes.hpp"
#include "lexer.hpp"

NodePtr parse(const std::vector<Token>& tokens);
std::string& print_ast(const NodePtr& node, std::string& output, const std::string& indent = "", bool last = true, bool is_value = false);

#endif // PARSER_HPP
