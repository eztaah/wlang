#include "asm_generator.hpp"
#include <iostream>

std::unordered_map<std::string, std::string> variables;
int offset;
int label_count;
std::vector<std::string> data_instructions;
std::vector<std::string> text_instructions;
std::vector<std::string> bss_instructions;

void generate_assembly_internal(const NodePtr &node);

std::string generate_assembly(const NodePtr &node)
{
    data_instructions.clear();
    text_instructions.clear();
    bss_instructions.clear();
    offset = 0;

    generate_assembly_internal(node);

    // Combine data, bss and text sections
    std::vector<std::string> combined_instructions = data_instructions;
    combined_instructions.insert(combined_instructions.end(), bss_instructions.begin(), bss_instructions.end());
    combined_instructions.insert(combined_instructions.end(), text_instructions.begin(), text_instructions.end());

    std::string asm_final_output = "";
    for (const std::string &instruction : combined_instructions) {
        asm_final_output += instruction + "\n";
    }

    return asm_final_output;
}

void generate_assembly_internal(const NodePtr &node)
{

    if (ProgramNode *pnode = dynamic_cast<ProgramNode *>(node.get())) {
        // setup data instructions
        data_instructions.push_back("\nsection .data");
        data_instructions.push_back("endp_printf_content: db \"%s\", 0");
        data_instructions.push_back("endp_str: db 10, 0");
        data_instructions.push_back("input_format: db \"%d\", 0");

        // setup bss instructions 
        bss_instructions.push_back("\nsection .bss");
        bss_instructions.push_back("box resb 64");
        bss_instructions.push_back("buffer resb 8");

        // setup text instructions 
        text_instructions.push_back("\nsection .text");
        text_instructions.push_back("global _start");
        text_instructions.push_back("extern printf");
        text_instructions.push_back("extern scanf");
        text_instructions.push_back("_start:");
        text_instructions.push_back("\n; initialisation");
        // text_instructions.push_back("push rbp");
        // text_instructions.push_back("mov r12, number");
        // text_instructions.push_back("mov rbp, rsp");

        for (const NodePtr &stmt : pnode->_statements) {
            generate_assembly_internal(stmt);
            offset -= 8;
        }
        // printf final
        text_instructions.push_back("; printf");
        text_instructions.push_back("mov rdi, endp_printf_content");
        text_instructions.push_back("mov rsi, endp_str");
        text_instructions.push_back("xor rax, rax");
        text_instructions.push_back("call printf");
        // exit
        text_instructions.push_back("\n; exit");
        // text_instructions.push_back("mov rsp, rbp");
        // text_instructions.push_back("pop rbp");
        text_instructions.push_back("mov rax, 60");
        text_instructions.push_back("mov rdi, 0");
        text_instructions.push_back("syscall");
    }

    else if (VarDeclNode *vnode = dynamic_cast<VarDeclNode *>(node.get())) {
        text_instructions.push_back("\n; variables declaration");
        std::string var_offset = "qword [box+" + std::to_string(abs(offset)) + "]";
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
        if (fnode->_name == "print") {
            text_instructions.push_back("\n; print");
            // Gestion de println
            for (const NodePtr &arg : fnode->_args) {
                if (StringNode *snode = dynamic_cast<StringNode *>(arg.get())) {
                    std::string str_label = "str_" + std::to_string(label_count++);
                    std::string format_label = "printf_content_" + std::to_string(label_count++);

                    // handle \n
                    std::string processed_content;
                    for (size_t i = 0; i < snode->_content.size(); ++i) {
                        if (snode->_content[i] == '\\' && snode->_content[i + 1] == 'n' && i + 1 < snode->_content.size()) {
                            processed_content += "\", 10, \"";
                            ++i;
                        }
                        else {
                            processed_content.push_back(snode->_content[i]);
                        }
                    }

                    data_instructions.push_back(format_label + ": db \"%s\", 0");
                    data_instructions.push_back(str_label + ": db \"" + processed_content + "\", 0");

                    // printf
                    text_instructions.push_back("mov rdi, " + format_label);
                    text_instructions.push_back("mov rsi, " + str_label);
                    text_instructions.push_back("xor rax, rax");
                    text_instructions.push_back("call printf");
                }
                else {
                    generate_assembly_internal(arg); // traite la numbernode
                    std::string format_label = "printf_content_" + std::to_string(label_count++);
                    data_instructions.push_back(format_label + ": db \"%d\", 0");

                    // printf
                    text_instructions.push_back("mov rdi, " + format_label);
                    text_instructions.push_back("mov rsi, rax");
                    text_instructions.push_back("xor rax, rax");
                    text_instructions.push_back("call printf");
                }
            }
        } 

        // gestion de la fonction input
        else if (fnode->_name == "input") {
            text_instructions.push_back("\n; input");

            std::string var_address = "box+" + std::to_string(std::abs(offset)); // Exemple, dépend de votre implémentation.

            text_instructions.push_back("mov rdi, input_format"); // Adresse du format.
            text_instructions.push_back("mov rsi, buffer");  // Adresse de la variable 'a'.
            text_instructions.push_back("xor rax, rax"); // Parce que scanf est une fonction variadique.
            text_instructions.push_back("call scanf");
            text_instructions.push_back("mov rax, [buffer]");

        }
        else {
            std::cout << "\033[31m[!] Generator error 1\033[0m" << std::endl;
            exit(-1);
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
