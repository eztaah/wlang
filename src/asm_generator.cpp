#include "asm_generator.hpp"
#include <iostream>

std::unordered_map<std::string, std::string> variables;
int offset;
int label_count;
std::vector<std::string> data_instructions;
std::vector<std::string> text_instructions;

void generate_assembly_internal(const NodePtr &node);

std::string generate_assembly(const NodePtr &node)
{
    data_instructions.clear();
    text_instructions.clear();
    offset = 0;

    generate_assembly_internal(node);

    // Combine data and text sections
    std::vector<std::string> combined_instructions = data_instructions;
    combined_instructions.insert(combined_instructions.end(), text_instructions.begin(), text_instructions.end());

    std::string asm_final_output = "section .data\n";
    for (const std::string &instruction : combined_instructions) {
        asm_final_output += instruction + "\n";
    }
    asm_final_output += "mov rax, 60\n";
    asm_final_output += "mov rdi, 0\n";
    asm_final_output += "syscall\n";

    return asm_final_output;
}

void generate_assembly_internal(const NodePtr &node)
{

    if (ProgramNode *pnode = dynamic_cast<ProgramNode *>(node.get())) {
        text_instructions.push_back("\nsection .text");
        text_instructions.push_back("global _start");
        text_instructions.push_back("extern printf");
        text_instructions.push_back("_start:");
        text_instructions.push_back("\n; initialisation");
        text_instructions.push_back("push rbp");
        text_instructions.push_back("mov rbp, rsp");
        text_instructions.push_back("sub rsp, 32");

        for (const NodePtr &stmt : pnode->_statements) {
            generate_assembly_internal(stmt);
            offset -= 8;
        }
        text_instructions.push_back("\n; exit");
        text_instructions.push_back("mov rsp, rbp");
        text_instructions.push_back("pop rbp");
    }

    else if (VarDeclNode *vnode = dynamic_cast<VarDeclNode *>(node.get())) {
        text_instructions.push_back("\n; variables declaration");
        std::string var_offset = "qword [rbp-" + std::to_string(abs(offset)) + "]";
        variables[vnode->_name] = var_offset;
        generate_assembly_internal(vnode->_value);
        text_instructions.push_back("mov " + var_offset + ", rax");
    }

    else if (VarModifNode *mnode = dynamic_cast<VarModifNode *>(node.get())) {
        text_instructions.push_back("\n; variables modification");

        // recuperation de l'adresse de la variable
        auto var_offset = variables.find(mnode->_name);
        // gere les erreurs
        if (var_offset == variables.end()) {
            std::cout << "Undefined variable " + mnode->_name << std::endl;
            exit(1);
        }
        generate_assembly_internal(mnode->_value);
        text_instructions.push_back("mov " + var_offset->second + ", rax");
    }

    else if (FunctionCallNode *fnode = dynamic_cast<FunctionCallNode *>(node.get())) {
        if (fnode->_name == "println") {
            text_instructions.push_back("\n; println");
            // Gestion de println
            for (const NodePtr &arg : fnode->_args) {
                if (StringNode *snode = dynamic_cast<StringNode *>(arg.get())) {
                    std::string str_label = "str_" + std::to_string(label_count++);
                    std::string format_label = "printf_content_" + std::to_string(label_count++);
                    data_instructions.push_back(format_label + ": db \"%s\", 10, 0");
                    data_instructions.push_back(str_label + ": db \"" + snode->_content + "\", 0");

                    // printf
                    text_instructions.push_back("mov rdi, " + format_label);
                    text_instructions.push_back("mov rsi, " + str_label);
                    text_instructions.push_back("xor rax, rax");
                    text_instructions.push_back("call printf");
                }
                else {
                    generate_assembly_internal(arg); // traite la numbernode
                    std::string format_label = "printf_content_" + std::to_string(label_count++);
                    data_instructions.push_back(format_label + ": db \"%d\", 10, 0");

                    // printf
                    text_instructions.push_back("mov rdi, " + format_label);
                    text_instructions.push_back("mov rsi, rax");
                    text_instructions.push_back("xor rax, rax");
                    text_instructions.push_back("call printf");
                }
            }
        }
    }

    else if (NumberNode *nnode = dynamic_cast<NumberNode *>(node.get())) {
        text_instructions.push_back("mov rax, " + std::to_string(nnode->_value));
    }

    else if (BinOpNode *bnode = dynamic_cast<BinOpNode *>(node.get())) {
        generate_assembly_internal(bnode->_right);
        text_instructions.push_back("push rax");
        generate_assembly_internal(bnode->_left);
        text_instructions.push_back("pop rbx");
        if (bnode->_op == PLUS) {
            text_instructions.push_back("add rax, rbx");
        }
        else if (bnode->_op == MINUS) {
            text_instructions.push_back("sub rax, rbx");
        }
        else if (bnode->_op == TIMES) {
            text_instructions.push_back("imul rax, rbx");
        }
        else if (bnode->_op == DIVIDE) {
            text_instructions.push_back("cqo");      // Convertit rax en rdx:rax, étendant le signe
            text_instructions.push_back("idiv rbx"); // Division de rdx:rax par rbx
        }
    }

    else if (BoolOpNode *bnode = dynamic_cast<BoolOpNode *>(node.get())) {
        generate_assembly_internal(bnode->_right);
        text_instructions.push_back("push rax");
        generate_assembly_internal(bnode->_left);
        text_instructions.push_back("pop rbx");

        std::string true_label = "if_true_" + std::to_string(label_count);

        if (bnode->_op == EQUALS_EQUALS) {
            text_instructions.push_back("cmp rax, rbx");
            text_instructions.push_back("je " + true_label);
        }
        else if (bnode->_op == NOT_EQUALS) {
            text_instructions.push_back("cmp rax, rbx");
            text_instructions.push_back("jne " + true_label);
        }
        else if (bnode->_op == LESS_THAN) {
            text_instructions.push_back("cmp rax, rbx");
            text_instructions.push_back("jl " + true_label);
        }
        else if (bnode->_op == LESS_THAN_EQUALS) {
            text_instructions.push_back("cmp rax, rbx");
            text_instructions.push_back("jle " + true_label);
        }
        else if (bnode->_op == GREATER_THAN) {
            text_instructions.push_back("cmp rax, rbx");
            text_instructions.push_back("jg " + true_label);
        }
        else if (bnode->_op == GREATER_THAN_EQUALS) {
            text_instructions.push_back("cmp rax, rbx");
            text_instructions.push_back("jge " + true_label);
        }
    }

    else if (VarRefNode *vrefnode = dynamic_cast<VarRefNode *>(node.get())) {
        auto var_offset = variables.find(vrefnode->_name);
        if (var_offset != variables.end()) {
            text_instructions.push_back("mov rax, " + var_offset->second);
        }
        else {
            std::cout << "Undefined variable " + vrefnode->_name << std::endl;
            exit(1);
        }
    }

    else if (IfNode *inode = dynamic_cast<IfNode *>(node.get())) {
        std::string true_label = "if_true_" + std::to_string(label_count);
        std::string end_label = "if_end_" + std::to_string(label_count);
        std::string else_label = "else_" + std::to_string(label_count);

        text_instructions.push_back("\n; if statement");
        generate_assembly_internal(inode->_condition);

        // si il y a un else
        if (inode->_false_block.empty() == false) {
            text_instructions.push_back("jmp " + else_label);
        }
        else {
            // si pas de else
            text_instructions.push_back("jmp " + end_label);
        }

        // true block
        text_instructions.push_back(true_label + ":");
        for (const NodePtr &stmt : inode->_true_block) {
            label_count++;
            generate_assembly_internal(stmt);
        }
        text_instructions.push_back("jmp " + end_label);
        // else block
        if (inode->_false_block.empty() == false) {
            text_instructions.push_back(else_label + ":");
            for (const NodePtr &stmt : inode->_false_block) {
                label_count++;
                generate_assembly_internal(stmt);
            }
        }
        text_instructions.push_back("jmp " + end_label);

        // end
        text_instructions.push_back(end_label + ":");
    }

    else if (WhileNode *wnode = dynamic_cast<WhileNode *>(node.get())) {
        std::string while_label = "while_label_" + std::to_string(label_count);
        std::string true_label = "if_true_" + std::to_string(label_count);
        std::string end_label = "if_end_" + std::to_string(label_count);
        std::string else_label = "else_" + std::to_string(label_count);

        text_instructions.push_back("\n; while statement");
        text_instructions.push_back(while_label + ":");
        generate_assembly_internal(wnode->_condition);
        text_instructions.push_back("jmp " + else_label);

        // true block
        text_instructions.push_back(true_label + ":");
        for (const NodePtr &stmt : wnode->_block) {
            label_count++;
            generate_assembly_internal(stmt);
        }
        text_instructions.push_back("jmp " + while_label);

        // else block
        text_instructions.push_back(else_label + ":");
        text_instructions.push_back("jmp " + end_label);

        // end
        text_instructions.push_back(end_label + ":");
    }
}
