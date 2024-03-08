#include <iostream>

#include "asm_generator.hpp"

std::unordered_map<std::string, std::string> variables;
int offset;
int string_label_count;
int if_label_count;
std::vector<std::string> data_instructions;
std::vector<std::string> bss_instructions;
std::vector<std::string> text_instructions;
std::vector<std::string> start_function_instructions;
bool printf_is_used = false;
bool print_string = false;
bool print_number = false;
bool scanf_is_used = false;

void generate_assembly_internal(const NodePtr &node);

std::string generate_assembly(const NodePtr &node)
{
    data_instructions.clear();
    text_instructions.clear();
    bss_instructions.clear();
    offset = 0;
    string_label_count = 0;
    if_label_count = 0;

    generate_assembly_internal(node);

    // Combine data, bss and text sections
    std::vector<std::string> combined_instructions = data_instructions;
    combined_instructions.insert(combined_instructions.end(), bss_instructions.begin(), bss_instructions.end());
    combined_instructions.insert(combined_instructions.end(), text_instructions.begin(), text_instructions.end());
    combined_instructions.insert(combined_instructions.end(), start_function_instructions.begin(), start_function_instructions.end());

    std::string asm_final_output = "";
    asm_final_output += "bits 64\n";
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

        // setup bss instructions
        bss_instructions.push_back("\nsection .bss");
        bss_instructions.push_back("box resb 64             ; we will store our variables here");

        // setup text instructions
        text_instructions.push_back("\nsection .text");
        text_instructions.push_back("global _start");
        start_function_instructions.push_back("\n_start:");

        for (const NodePtr &stmt : pnode->_statements) {
            generate_assembly_internal(stmt);
            // offset += 8;
        }

        // add extern printf or/and scanf if used
        if (printf_is_used) {
            text_instructions.push_back("extern printf");
            data_instructions.push_back("string_placeholder: db \"%s\", 0");

            // printf final (bug \n)
            data_instructions.push_back("endp_str: db 10, 0");
            start_function_instructions.push_back("; print \\n before exiting to prevent display bug");
            start_function_instructions.push_back("mov rdi, string_placeholder");
            start_function_instructions.push_back("mov rsi, endp_str");
            start_function_instructions.push_back("xor rax, rax");
            start_function_instructions.push_back("call printf");
        }

        if (scanf_is_used || print_number) {
            data_instructions.push_back("number_placeholder: db \"%d\", 0");
        }
        if (scanf_is_used) {
            bss_instructions.push_back("scanf_buffer resb 8");
            text_instructions.push_back("extern scanf");
        }

        // exit
        start_function_instructions.push_back("\n; exit");
        start_function_instructions.push_back("mov rax, 60");
        start_function_instructions.push_back("mov rdi, 0");
        start_function_instructions.push_back("syscall");
    }

    else if (VarDeclNode *vnode = dynamic_cast<VarDeclNode *>(node.get())) {
        start_function_instructions.push_back("\n; variables declaration");
        std::string var_offset = "qword [box+" + std::to_string(abs(offset)) + "]";
        offset += 8;
        variables[vnode->_name] = var_offset;
        generate_assembly_internal(vnode->_value);
        start_function_instructions.push_back("mov " + var_offset + ", rax");
    }

    else if (VarModifNode *mnode = dynamic_cast<VarModifNode *>(node.get())) {
        start_function_instructions.push_back("\n; variables modification");

        // recuperation de l'adresse de la variable
        auto var_offset = variables.find(mnode->_name);
        // gere les erreurs
        if (var_offset == variables.end()) {
            std::cout << "Undefined variable " + mnode->_name << std::endl;
            exit(1);
        }
        generate_assembly_internal(mnode->_value);
        start_function_instructions.push_back("mov " + var_offset->second + ", rax");
    }

    else if (FunctionCallNode *fnode = dynamic_cast<FunctionCallNode *>(node.get())) {
        if (fnode->_name == "print") {
            printf_is_used = true;
            start_function_instructions.push_back("\n; print");
            // Gestion de println
            for (const NodePtr &arg : fnode->_args) {
                if (StringNode *snode = dynamic_cast<StringNode *>(arg.get())) {
                    std::string str_label = "str_" + std::to_string(string_label_count);
                    string_label_count++;

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
                    data_instructions.push_back(str_label + ": db \"" + processed_content + "\", 0");

                    // printf
                    start_function_instructions.push_back("mov rdi, string_placeholder");
                    start_function_instructions.push_back("mov rsi, " + str_label);
                    start_function_instructions.push_back("xor rax, rax");
                    start_function_instructions.push_back("call printf");
                }
                else {
                    generate_assembly_internal(arg); // traite la numbernode
                    print_number = true;

                    // printf
                    start_function_instructions.push_back("mov rdi, number_placeholder");
                    start_function_instructions.push_back("mov rsi, rax");
                    start_function_instructions.push_back("xor rax, rax");
                    start_function_instructions.push_back("call printf");
                }
            }
            start_function_instructions.push_back("");
        }

        // gestion de la fonction input
        else if (fnode->_name == "input") {
            scanf_is_used = true;
            start_function_instructions.push_back("\n; input");

            std::string var_address = "box+" + std::to_string(offset);

            start_function_instructions.push_back("mov rdi, number_placeholder");
            start_function_instructions.push_back("mov rsi, scanf_buffer");
            start_function_instructions.push_back("xor rax, rax");
            start_function_instructions.push_back("call scanf");
            start_function_instructions.push_back("mov rax, [scanf_buffer]");
        }
        else {
            std::cout << "\033[31m[!] Generator error 1\033[0m" << std::endl;
            exit(-1);
        }
    }

    else if (NumberNode *nnode = dynamic_cast<NumberNode *>(node.get())) {
        start_function_instructions.push_back("mov rax, " + std::to_string(nnode->_value));
    }

    else if (BinOpNode *bnode = dynamic_cast<BinOpNode *>(node.get())) {
        start_function_instructions.push_back("; < binOp");
        generate_assembly_internal(bnode->_right);
        start_function_instructions.push_back("push rax");
        generate_assembly_internal(bnode->_left);
        start_function_instructions.push_back("pop rbx");

        switch (bnode->_op) {
        case EQUALS_EQUALS:
            start_function_instructions.push_back("cmp rax, rbx");
            start_function_instructions.push_back("sete al");       // met 1 dans al (partie basse de rax), sinon 0
            start_function_instructions.push_back("movzx rax, al"); // etend la valeur de al à rax
            break;

        case NOT_EQUALS:
            start_function_instructions.push_back("cmp rax, rbx");
            start_function_instructions.push_back("setne al");
            start_function_instructions.push_back("movzx rax, al");
            break;

        case LESS_THAN:
            start_function_instructions.push_back("cmp rax, rbx");
            start_function_instructions.push_back("setl al");
            start_function_instructions.push_back("movzx rax, al");
            break;

        case LESS_THAN_EQUALS:
            start_function_instructions.push_back("cmp rax, rbx");
            start_function_instructions.push_back("setle al");
            start_function_instructions.push_back("movzx rax, al");
            break;

        case GREATER_THAN:
            start_function_instructions.push_back("cmp rax, rbx");
            start_function_instructions.push_back("setg al");
            start_function_instructions.push_back("movzx rax, al");
            break;

        case GREATER_THAN_EQUALS:
            start_function_instructions.push_back("cmp rax, rbx");
            start_function_instructions.push_back("setge al");
            start_function_instructions.push_back("movzx rax, al");
            break;

        case BIN_OR:
            start_function_instructions.push_back("or rax, rbx");
            break;

        case XOR:
            start_function_instructions.push_back("xor rax, rbx");
            break;

        case BIN_AND:
            start_function_instructions.push_back("and rax, rbx");
            break;

        case SHIFT_LEFT:
            start_function_instructions.push_back("mov cl, bl");
            start_function_instructions.push_back("shl rax, cl"); // cl est un registre 8 bits
            break;

        case SHIFT_RIGHT:
            start_function_instructions.push_back("mov cl, bl");
            start_function_instructions.push_back("shr rax, cl"); // cl est un registre 8 bits
            break;

        case PLUS:
            start_function_instructions.push_back("add rax, rbx");
            break;

        case MINUS:
            start_function_instructions.push_back("sub rax, rbx");
            break;

        case TIMES:
            start_function_instructions.push_back("imul rax, rbx");
            break;

        case DIVIDE:
            start_function_instructions.push_back("cqo");      // Convertit rax en rdx:rax, étendant le signe
            start_function_instructions.push_back("idiv rbx"); // Division de rdx:rax par rbx, resulat dans rax, reste dans rdx
            break;

        case MODULO:
            start_function_instructions.push_back("cqo");          // Convertit rax en rdx:rax, étendant le signe
            start_function_instructions.push_back("idiv rbx");     // Division de rdx:rax par rbx, resulat dans rax, reste dans rdx
            start_function_instructions.push_back("mov rax, rdx"); // met reste division dans rax
            break;

        default:
            display_and_throw_internal_error("unknow operator in BinOpNode");
            exit(1);
        }
        start_function_instructions.push_back("; binOp >");
    }

    else if (VarRefNode *vrefnode = dynamic_cast<VarRefNode *>(node.get())) {
        auto var_offset = variables.find(vrefnode->_name);
        if (var_offset != variables.end()) {
            start_function_instructions.push_back("mov rax, " + var_offset->second);
        }
        else {
            display_and_throw_internal_error("undefined variable");
            exit(1);
        }
    }

    else if (IfNode *inode = dynamic_cast<IfNode *>(node.get())) {
        std::string true_label = "if_true_" + std::to_string(if_label_count);
        std::string end_label = "if_end_" + std::to_string(if_label_count);
        std::string else_label = "else_" + std::to_string(if_label_count);
        if_label_count++;

        start_function_instructions.push_back("\n; if statement");
        generate_assembly_internal(inode->_condition);

        start_function_instructions.push_back("cmp rax, 0");
        start_function_instructions.push_back("jne " + true_label);

        // si il y a un else
        if (inode->_false_block.empty() == false) {
            start_function_instructions.push_back("jmp " + else_label);
        }
        else {
            // si pas de else
            start_function_instructions.push_back("jmp " + end_label);
        }

        // true block
        start_function_instructions.push_back(true_label + ":");
        for (const NodePtr &stmt : inode->_true_block) {
            generate_assembly_internal(stmt);
        }
        start_function_instructions.push_back("jmp " + end_label);
        // else block
        if (inode->_false_block.empty() == false) {
            start_function_instructions.push_back(else_label + ":");
            for (const NodePtr &stmt : inode->_false_block) {
                generate_assembly_internal(stmt);
            }
        }

        // end
        start_function_instructions.push_back(end_label + ":");
        start_function_instructions.push_back("");
    }

    else if (WhileNode *wnode = dynamic_cast<WhileNode *>(node.get())) {
        std::string while_label = "while_label_" + std::to_string(if_label_count);
        std::string true_label = "if_true_" + std::to_string(if_label_count);
        std::string end_label = "if_end_" + std::to_string(if_label_count);
        if_label_count++;

        start_function_instructions.push_back("\n; while statement");
        start_function_instructions.push_back(while_label + ":");
        generate_assembly_internal(wnode->_condition);

        start_function_instructions.push_back("cmp rax, 1");
        start_function_instructions.push_back("je " + true_label);
        start_function_instructions.push_back("jmp " + end_label);

        // true block
        start_function_instructions.push_back(true_label + ":");
        for (const NodePtr &stmt : wnode->_block) {
            generate_assembly_internal(stmt);
        }
        start_function_instructions.push_back("jmp " + while_label);

        // end
        start_function_instructions.push_back(end_label + ":");
    }

    else {
        display_and_throw_internal_error("unknow node encontered");
        exit(1);
    }
}
