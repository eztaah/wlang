// parser.hpp
#ifndef PARSER_HH
#define PARSER_HH

#include <memory>
#include <string>
#include <vector>

#include "global.hh"

NodePtr parse(const std::vector<Token> &tokens);
std::string &print_ast(const NodePtr &node, std::string &output, const std::string &indent = "", bool last = true, bool is_value = false);

#endif // PARSER_HH
