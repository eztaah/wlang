#include "semantic_analysis.hh"
#include <stdexcept>
#include <vector>

void visitNode(const NodePtr &node);
void visitFunctionCall(const FunctionCallNode *node);

void analyzeAST(const NodePtr &node)
{
    visitNode(node);
}

void visitNode(const NodePtr &node)
{
    if (auto fnode = dynamic_cast<const FunctionCallNode *>(node.get())) {
        visitFunctionCall(fnode);
    }

    else if (auto inode = dynamic_cast<const IfNode *>(node.get())) {
        visitNode(inode->_condition);
        for (const auto &stmt : inode->_true_block) {
            visitNode(stmt);
        }
        for (const auto &stmt : inode->_false_block) {
            visitNode(stmt);
        }
    }
}

void visitFunctionCall(const FunctionCallNode *node)
{
    if (node->_name != "println") {
        throw std::runtime_error("Fonction non autorisée appelée: " + node->_name);
    }
    for (const auto &arg : node->_args) {
        visitNode(arg);
    }
}
