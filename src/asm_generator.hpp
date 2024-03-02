#ifndef ASSEMBLY_GENERATOR_HPP
#define ASSEMBLY_GENERATOR_HPP

#include "nodes.hpp"
#include <vector>
#include <string>
#include <unordered_map>

std::pair<std::vector<std::string>, int> generate_assembly(const NodePtr& node, std::unordered_map<std::string, std::string>& variables, int offset = 0, int label_count = 0);

#endif // ASSEMBLY_GENERATOR_HPP
