#include <iostream>
#include "asm_generator.hpp"

std::pair<std::vector<std::string>, int> generate_assembly(const NodePtr& node, std::unordered_map<std::string, std::string>& variables, int offset, int label_count)
{
    std::vector<std::string> instructions;
    int current_label_count = label_count;

    if (ProgramNode* pnode = dynamic_cast<ProgramNode*>(node.get())) {
        instructions.push_back(";rbp est fixe");
        instructions.push_back("push rbp");
        instructions.push_back("mov rbp, rsp\n");
        for (const NodePtr& stmt : pnode->_statements) {
            auto [stmt_instructions, new_label_count] = generate_assembly(stmt, variables, offset, label_count);
            instructions.insert(instructions.end(), stmt_instructions.begin(), stmt_instructions.end());
            offset -= 8;
            label_count = new_label_count;
        }
        instructions.push_back("\nmov rsp, rbp");
        instructions.push_back("pop rbp");
    } 
    else if (VarDeclNode* vnode = dynamic_cast<VarDeclNode*>(node.get())) {
        std::string var_offset = "[rbp-" + std::to_string(abs(offset)) + "]";
        variables[vnode->_name] = var_offset;
        auto [value_instructions, new_label_count] = generate_assembly(vnode->_value, variables, offset, label_count);
        instructions.insert(instructions.end(), value_instructions.begin(), value_instructions.end());
        instructions.push_back("mov " + var_offset + ", rax");
        label_count = new_label_count;
    } 
    else if (NumberNode* nnode = dynamic_cast<NumberNode*>(node.get())) {
        instructions.push_back("mov rax, " + std::to_string(nnode->_value));
    } 
    else if (BinOpNode* bnode = dynamic_cast<BinOpNode*>(node.get())) {
        auto [right_instructions, right_label_count] = generate_assembly(bnode->_right, variables, offset, label_count);
        instructions.insert(instructions.end(), right_instructions.begin(), right_instructions.end());
        instructions.push_back("push rax");
        auto [left_instructions, left_label_count] = generate_assembly(bnode->_left, variables, offset, right_label_count);
        instructions.insert(instructions.end(), left_instructions.begin(), left_instructions.end());
        instructions.push_back("pop rbx");
        if (bnode->_op == "PLUS") {
            instructions.push_back("add rax, rbx");
        }
        label_count = left_label_count;
    } 
    else if (VarRefNode* vrefnode = dynamic_cast<VarRefNode*>(node.get())) {
        auto var_offset = variables.find(vrefnode->_name);
        if (var_offset != variables.end()) {
            instructions.push_back("mov rax, " + var_offset->second);
        } else {
            std::cout << "Undefined variable " + vrefnode->_name << std::endl;
            exit(1);
        }
    } 
    else if (IfNode* inode = dynamic_cast<IfNode*>(node.get())) {
        auto [condition_instructions, cond_label_count] = generate_assembly(inode->_condition, variables, offset, label_count);
        instructions.insert(instructions.end(), condition_instructions.begin(), condition_instructions.end());
        instructions.push_back("cmp rax, 0");
        std::string true_label = "if_true_" + std::to_string(current_label_count);
        std::string end_label = "if_end_" + std::to_string(current_label_count);
        instructions.push_back("je " + true_label);
        for (const NodePtr& stmt : inode->_true_block) {
            auto [true_block_instructions, block_label_count] = generate_assembly(stmt, variables, offset, cond_label_count + 1);
            instructions.insert(instructions.end(), true_block_instructions.begin(), true_block_instructions.end());
            label_count = block_label_count;
        }
        instructions.push_back("jmp " + end_label);
        instructions.push_back(true_label + ":");
        instructions.push_back(end_label + ":");
    }

    return {instructions, label_count};
}
