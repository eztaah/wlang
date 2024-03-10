#include <iostream>

#include "asm_generator.hh"

std::unordered_map<std::string, std::string> variables;
int offset;
int string_label_count;
int global_label_count;
std::vector<std::string> data_instructions;
std::vector<std::string> bss_instructions;
std::vector<std::string> text_instructions;
std::vector<std::string> start_function_instructions;
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
    global_label_count = 0;

    generate_assembly_internal(node);

    // Combine data, bss and text sections
    std::vector<std::string> combined_instructions = data_instructions;
    combined_instructions.insert(combined_instructions.end(), bss_instructions.begin(), bss_instructions.end());
    combined_instructions.insert(combined_instructions.end(), text_instructions.begin(), text_instructions.end());
    combined_instructions.insert(combined_instructions.end(), start_function_instructions.begin(), start_function_instructions.end());

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
        if (dev_mode) {
            data_instructions.push_back("    # --- SECTION DATA ---");
        }
        data_instructions.push_back("    .section .data");

        // setup bss instructions
        if (dev_mode) {
            bss_instructions.push_back("\n\n    # --- SECTION BSS ---");
        }
        bss_instructions.push_back("    .section .bss");

        // setup text instructions
        if (dev_mode) {
            text_instructions.push_back("\n\n    # --- SECTION TEXT ---");
        }
        text_instructions.push_back("    .section .text");
        text_instructions.push_back("    .global _start");
        if (dev_mode) {
            start_function_instructions.push_back("\n_start:");
        }
        else {
            start_function_instructions.push_back("_start:");
        }
        if (dev_mode) {
            start_function_instructions.push_back("    # intialisation");
        }
        start_function_instructions.push_back("    pushq   %rbp");
        start_function_instructions.push_back("    movq    %rsp, %rbp");
        start_function_instructions.push_back("    subq    $88, %rsp"); //  you can store 11 variables

        for (const NodePtr &stmt : pnode->_statements) {
            generate_assembly_internal(stmt);
        }

        // add extern printf or/and scanf if used
        if (print_number || print_string) {
            print_string = true;
            text_instructions.push_back("    .extern printf");

            data_instructions.push_back(".Sn:\n    .string \"\\n\"");

            if (dev_mode) {
                start_function_instructions.push_back("\n    # print \\n before exiting to prevent display bug");
            }
            start_function_instructions.push_back("    movq    $.Sn, %rsi");
            start_function_instructions.push_back("    movq    $.SP, %rdi");
            start_function_instructions.push_back("    xor     %rax, %rax");
            start_function_instructions.push_back("    call    printf");
        }
        if (print_string) {
            data_instructions.push_back(".SP:\n    .string \"%s\"");
        }
        if (scanf_is_used || print_number) {
            data_instructions.push_back(".NP:\n    .string \"%d\"");
        }
        if (scanf_is_used) {
            bss_instructions.push_back("buff:\n    .space 8");
            text_instructions.push_back("    .extern scanf");
        }

        // exit
        if (dev_mode) {
            start_function_instructions.push_back("\n.ENDP:");
        }
        else {
            start_function_instructions.push_back(".ENDP:");
        }
        if (dev_mode) {
            start_function_instructions.push_back("    # end of the program");
        }
        start_function_instructions.push_back("    movq    %rbp, %rsp");
        start_function_instructions.push_back("    pop     %rbp");
        if (dev_mode) {
            start_function_instructions.push_back("    # exit syscall");
        }
        start_function_instructions.push_back("    movq    $60, %rax");
        start_function_instructions.push_back("    xor     %rdi, %rdi");
        start_function_instructions.push_back("    syscall");
    }

    else if (VarDeclNode *vnode = dynamic_cast<VarDeclNode *>(node.get())) {
        if (dev_mode) {
            start_function_instructions.push_back("\n    # variables declaration");
        }
        std::string var_offset = std::to_string(offset) + "(%rbp)";
        variables[vnode->_name] = var_offset;
        generate_assembly_internal(vnode->_value);
        start_function_instructions.push_back("    movq    %rax, " + var_offset);
        offset -= 8;
    }

    else if (VarModifNode *mnode = dynamic_cast<VarModifNode *>(node.get())) {
        if (dev_mode) {
            start_function_instructions.push_back("\n    # variables modification");
        }
        // recuperation de l'adresse de la variable
        auto var_offset = variables.find(mnode->_name);
        // gere les erreurs
        if (var_offset == variables.end()) {
            display_and_throw_error("undefined variable", -1);
            exit(1);
        }
        generate_assembly_internal(mnode->_value);
        start_function_instructions.push_back("    movq    %rax, " + var_offset->second);
    }

    else if (FunctionCallNode *fnode = dynamic_cast<FunctionCallNode *>(node.get())) {
        if (fnode->_name == "print") {
            if (dev_mode) {
                start_function_instructions.push_back("\n    # print");
            }
            // Gestion de println
            for (const NodePtr &arg : fnode->_args) {
                if (StringNode *snode = dynamic_cast<StringNode *>(arg.get())) {
                    print_string = true;
                    std::string str_label = ".S" + std::to_string(string_label_count);
                    string_label_count++;

                    // handle \n
                    std::string processed_content;

                    for (size_t i = 0; i < snode->_content.size(); ++i) {
                        processed_content.push_back(snode->_content[i]);
                    }
                    data_instructions.push_back(str_label + ":\n    .string \"" + processed_content + "\"");

                    // printf
                    start_function_instructions.push_back("    movq    $" + str_label + ", %rsi");
                    start_function_instructions.push_back("    movq    $.SP, %rdi");
                    start_function_instructions.push_back("    xor     %rax, %rax");
                    start_function_instructions.push_back("    call    printf");
                }
                else {
                    generate_assembly_internal(arg); // traite la numbernode
                    print_number = true;

                    // printf
                    start_function_instructions.push_back("    movq    %rax, %rsi");
                    start_function_instructions.push_back("    movq    $.NP, %rdi");
                    start_function_instructions.push_back("    xor     %rax, %rax");
                    start_function_instructions.push_back("    call    printf");
                }
            }
            // start_function_instructions.push_back("");
        }

        // gestion de la fonction input
        else if (fnode->_name == "input") {
            scanf_is_used = true;
            if (dev_mode) {
                start_function_instructions.push_back("\n    # input");
            }
            start_function_instructions.push_back("    movq    $buff, %rsi");
            start_function_instructions.push_back("    movq    $.NP, %rdi");
            start_function_instructions.push_back("    xor     %rax, %rax");
            start_function_instructions.push_back("    call    scanf");
            start_function_instructions.push_back("    movq    buff(%rip), %rax");
        }
        // gestion de la fonction exit
        else if (fnode->_name == "exit") {
            start_function_instructions.push_back("    jmp    .ENDP");
        }
        else {
            display_and_throw_internal_error("generator error");
            exit(-1);
        }
    }

    else if (NumberNode *nnode = dynamic_cast<NumberNode *>(node.get())) {
        start_function_instructions.push_back("    movq    $" + std::to_string(nnode->_value) + ", %rax");
    }

    else if (BinOpNode *bnode = dynamic_cast<BinOpNode *>(node.get())) {
        if (dev_mode) {
            start_function_instructions.push_back("    # < binOp");
        }
        generate_assembly_internal(bnode->_right);
        start_function_instructions.push_back("    pushq   %rax");
        generate_assembly_internal(bnode->_left);
        start_function_instructions.push_back("    pop     %rbx");

        switch (bnode->_op) {
        case EQUALS_EQUALS:
            start_function_instructions.push_back("    cmp     %rbx, %rax");
            start_function_instructions.push_back("    sete    %al");       // met 1 dans %al (partie basse de rax), sinon 0
            start_function_instructions.push_back("    movzbl  %al, %eax"); // etend la valeur de %al à rax
            break;

        case NOT_EQUALS:
            start_function_instructions.push_back("    cmp     %rbx, %rax");
            start_function_instructions.push_back("    setne   %al");
            start_function_instructions.push_back("    movzbl  %al, %eax");
            break;

        case LESS_THAN:
            start_function_instructions.push_back("    cmp     %rbx, %rax");
            start_function_instructions.push_back("    setl    %al");
            start_function_instructions.push_back("    movzbl  %al, %eax");
            break;

        case LESS_THAN_EQUALS:
            start_function_instructions.push_back("    cmp     %rbx, %rax");
            start_function_instructions.push_back("    setle   %al");
            start_function_instructions.push_back("    movzbl  %al, %eax");
            break;

        case GREATER_THAN:
            start_function_instructions.push_back("    cmp     %rbx, %rax");
            start_function_instructions.push_back("    setg    %al");
            start_function_instructions.push_back("    movzbl  %al, %eax"); // movzbl deplace un registre 8 bits vers 32 bits
            break;

        case GREATER_THAN_EQUALS:
            start_function_instructions.push_back("    cmp     %rbx, %rax");
            start_function_instructions.push_back("    setge   %al");
            start_function_instructions.push_back("    movzbl  %al, %eax");
            break;

        case BIN_OR:
            start_function_instructions.push_back("    or      %rbx, %rax");
            break;

        case AND:
            start_function_instructions.push_back("    and     %rbx, %rax");
            break;

        case OR:
            start_function_instructions.push_back("    or      %rbx, %rax");
            break;

        case XOR:
            start_function_instructions.push_back("    xor     %rbx, %rax");
            break;

        case BIN_AND:
            start_function_instructions.push_back("    and     %rbx, %rax");
            break;

        case SHIFT_LEFT:
            start_function_instructions.push_back("    mov     %bl, %cl");
            start_function_instructions.push_back("    shl     %cl, %rax"); // cl est un registre 8 bits
            break;

        case SHIFT_RIGHT:
            start_function_instructions.push_back("    mov     %bl, %cl");
            start_function_instructions.push_back("    shr     %cl, %rax"); // cl est un registre 8 bits
            break;

        case PLUS:
            start_function_instructions.push_back("    add     %rbx, %rax");
            break;

        case MINUS:
            start_function_instructions.push_back("    sub     %rbx, %rax");
            break;

        case TIMES:
            start_function_instructions.push_back("    imul    %rbx, %rax");
            break;

        case DIVIDE:
            start_function_instructions.push_back("    cqo");          // Convertit rax en rdx:rax, étendant le signe
            start_function_instructions.push_back("    idiv    %rbx"); // Division de rdx:rax par rbx, resulat dans rax, reste dans rdx
            break;

        case MODULO:
            start_function_instructions.push_back("    cqo");                // Convertit rax en rdx:rax, étendant le signe
            start_function_instructions.push_back("    idiv    %rbx");       // Division de rdx:rax par rbx, resulat dans rax, reste dans rdx
            start_function_instructions.push_back("    mov     %rdx, %rax"); // met reste division dans rax
            break;

        default:
            display_and_throw_internal_error("unknow operator in BinOpNode");
            exit(1);
        }
        if (dev_mode) {
            start_function_instructions.push_back("    # binOp >");
        }
    }

    else if (VarRefNode *vrefnode = dynamic_cast<VarRefNode *>(node.get())) {
        auto var_offset = variables.find(vrefnode->_name);
        if (var_offset != variables.end()) {
            start_function_instructions.push_back("    movq    " + var_offset->second + ", %rax");
        }
        else {
            display_and_throw_error("undefined variable", -1);
            exit(1);
        }
    }

    else if (IfNode *inode = dynamic_cast<IfNode *>(node.get())) {
        std::string true_label = ".LTRUE" + std::to_string(global_label_count);
        std::string end_label = ".LEND" + std::to_string(global_label_count);
        std::string else_label = ".LELSE" + std::to_string(global_label_count);
        global_label_count++;

        if (dev_mode) {
            start_function_instructions.push_back("\n    # if statement");
        }
        generate_assembly_internal(inode->_condition);

        start_function_instructions.push_back("    cmp     $0, %rax");
        start_function_instructions.push_back("    jne     " + true_label);

        // si il y a un else
        if (inode->_false_block.empty() == false) {
            start_function_instructions.push_back("    jmp     " + else_label);
        }
        else {
            // si pas de else
            start_function_instructions.push_back("    jmp     " + end_label);
        }

        // true block
        start_function_instructions.push_back(true_label + ":");
        for (const NodePtr &stmt : inode->_true_block) {
            generate_assembly_internal(stmt);
        }
        start_function_instructions.push_back("    jmp     " + end_label);
        // else block
        if (inode->_false_block.empty() == false) {
            start_function_instructions.push_back(else_label + ":");
            for (const NodePtr &stmt : inode->_false_block) {
                generate_assembly_internal(stmt);
            }
        }

        // end
        start_function_instructions.push_back(end_label + ":");
        // start_function_instructions.push_back("");
    }

    else if (WhileNode *wnode = dynamic_cast<WhileNode *>(node.get())) {
        std::string while_label = ".WHILE" + std::to_string(global_label_count);
        std::string true_label = ".LTRUE" + std::to_string(global_label_count);
        std::string end_label = ".LEND" + std::to_string(global_label_count);
        global_label_count++;

        if (dev_mode) {
            start_function_instructions.push_back("\n    # while statement");
        }
        start_function_instructions.push_back(while_label + ":");
        generate_assembly_internal(wnode->_condition);

        start_function_instructions.push_back("    cmp     $1, %rax");
        start_function_instructions.push_back("    je      " + true_label);
        start_function_instructions.push_back("    jmp     " + end_label);

        // true block
        start_function_instructions.push_back(true_label + ":");
        for (const NodePtr &stmt : wnode->_block) {
            generate_assembly_internal(stmt);
        }
        start_function_instructions.push_back("    jmp     " + while_label);

        // end
        start_function_instructions.push_back(end_label + ":");
    }

    else {
        display_and_throw_internal_error("unknow node encontered");
        exit(1);
    }
}
