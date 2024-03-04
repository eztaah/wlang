#ifndef ASSEMBLY_GENERATOR_HPP
#define ASSEMBLY_GENERATOR_HPP

#include "nodes.hpp"
#include <string>
#include <unordered_map>
#include <vector>

std::string generate_assembly(const NodePtr &node);

#endif // ASSEMBLY_GENERATOR_HPP
