#include "compiler.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Generator {
    const List* node_list;
    I32 var_decl_offset;
    List* asm_data;
    List* asm_bss;
    List* asm_text;
    List* asm_start;
    List* variables; // List of strings (variable offsets)
} Generator;

// Function prototypes
static Void generate_expression_node(Generator* generator, const ExprNode* expr_node);

static Generator* generator_new(const List* node_list)
{
    Generator* generator = calloc(1, sizeof(Generator));
    generator->node_list = node_list;
    generator->asm_data = list_new(sizeof(Char*));
    generator->asm_bss = list_new(sizeof(Char*));
    generator->asm_text = list_new(sizeof(Char*));
    generator->asm_start = list_new(sizeof(Char*));
    generator->variables = list_new(sizeof(Char*));
    return generator;
}

static Void generator_free(Generator* generator)
{
    list_free(generator->asm_data);
    list_free(generator->asm_bss);
    list_free(generator->asm_text);
    list_free(generator->asm_start);
    free(generator->variables);
    free(generator);
}

static Char* concat_instructions(List* instructions)
{
    Char* result = str_new("");
    for (I32 i = 0; i < instructions->size; i++) {
        result = str_cat(result, (Char*)instructions->items[i]);
        result = str_cat(result, "\n");
    }
    return result;
}

static Void generate_number_node(Generator* generator, const NumberNode* nnode)
{
    Char* instruction = str_new("    movq    $");
    instruction = str_cat(instruction, nnode->value);
    instruction = str_cat(instruction, ", %rax");

    list_push(generator->asm_start, instruction);
}

static Void generate_binop_node(Generator* generator, const BinopNode* bnode)
{
    if (dev_mode) {
        list_push(generator->asm_start, str_new("    # < binOp"));
    }

    generate_expression_node(generator, bnode->right);
    list_push(generator->asm_start, str_new("    pushq   %rax"));
    generate_expression_node(generator, bnode->left);
    list_push(generator->asm_start, str_new("    pop     %rbx"));

    switch (bnode->op) {
        case TOKEN_PLUS:
            list_push(generator->asm_start, str_new("    add     %rbx, %rax"));
            break;
        case TOKEN_MINUS:
            list_push(generator->asm_start, str_new("    sub     %rbx, %rax"));
            break;
        case TOKEN_MUL:
            list_push(generator->asm_start, str_new("    imul    %rbx, %rax"));
            break;
        case TOKEN_DIV:
            list_push(generator->asm_start, str_new("    cqo"));
            list_push(generator->asm_start, str_new("    idiv    %rbx"));
            break;
        default:
            UNREACHABLE();
    }

    if (dev_mode) {
        list_push(generator->asm_start, str_new("    # binOp >"));
    }
}

static Void generate_expression_node(Generator* generator, const ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_NUMBER:
            generate_number_node(generator, &expr_node->number_node);
            break;
        case NODE_BINOP:
            generate_binop_node(generator, &expr_node->bin_op_node);
            break;
        default:
            UNREACHABLE();
    }
}

static Void generate_var_decl_node(Generator* generator, const VarDeclNode* vnode)
{
    if (dev_mode) {
        list_push(generator->asm_start, str_new("\n    # variables declaration"));
    }
    Char* var_offset = malloc(20);
    sprintf(var_offset, "%d(%%rbp)", generator->var_decl_offset);
    list_push(generator->variables, var_offset);

    generate_expression_node(generator, vnode->value);

    Char* instruction = str_new("    movq    %rax, ");
    instruction = str_cat(instruction, var_offset);

    list_push(generator->asm_start, instruction);

    generator->var_decl_offset -= 8;
}

void ininitalize_asm_instructions(Generator* generator)
{
    // Setup initial instructions
    if (dev_mode) {
        list_push(generator->asm_data, str_new("    # --- SECTION DATA ---"));
    }
    list_push(generator->asm_data, str_new("    .section .data"));

    if (dev_mode) {
        list_push(generator->asm_bss, str_new("\n\n    # --- SECTION BSS ---"));
    }
    list_push(generator->asm_bss, str_new("    .section .bss"));

    if (dev_mode) {
        list_push(generator->asm_text, str_new("\n\n    # --- SECTION TEXT ---"));
    }
    list_push(generator->asm_text, str_new("    .section .text"));
    list_push(generator->asm_text, str_new("    .global _start"));

    if (dev_mode) {
        list_push(generator->asm_start, str_new("\n_start:"));
    }
    else {
        list_push(generator->asm_start, str_new("_start:"));
    }

    if (dev_mode) {
        list_push(generator->asm_start, str_new("    # intialisation"));
    }

    list_push(generator->asm_start, str_new("    pushq   %rbp"));       // save old stack pointer ?
    list_push(generator->asm_start, str_new("    movq    %rsp, %rbp")); // ?????

    // Pépare la pile pour etre capable d'acceuillir toutes les variables déclarées dans le code
    I32 stack_space = generator->node_list->size * 8;
    if (generator->node_list->size % 2 == 0) {
        stack_space += 8;
    }
    Char stack_space_str[20];
    sprintf(stack_space_str, "%d", stack_space);

    Char* instruction = str_new("    subq    $");
    instruction = str_cat(instruction, stack_space_str);
    instruction = str_cat(instruction, ", %rsp");
    list_push(generator->asm_start, instruction);
}

Void finalize_asm_instructions(Generator* generator)
{
    // exit
    if (dev_mode) {
        list_push(generator->asm_start, str_new("\n.ENDP:"));
    }
    else {
        list_push(generator->asm_start, str_new(".ENDP:"));
    }

    if (dev_mode) {
        list_push(generator->asm_start, str_new("    # end of the program"));
    }
    list_push(generator->asm_start, str_new("    movq    %rbp, %rsp"));
    list_push(generator->asm_start, str_new("    pop     %rbp"));
    if (dev_mode) {
        list_push(generator->asm_start, str_new("    # exit syscall"));
    }
    list_push(generator->asm_start, str_new("    movq    $60, %rax"));
    list_push(generator->asm_start, str_new("    xor     %rdi, %rdi"));
    list_push(generator->asm_start, str_new("    syscall"));
}

Char* convert_instructionlist_to_string(Generator* generator)
{
    // Combine all instructions into a single output string
    Char* data_section = concat_instructions(generator->asm_data);
    Char* bss_section = concat_instructions(generator->asm_bss);
    Char* text_section = concat_instructions(generator->asm_text);
    Char* start_section = concat_instructions(generator->asm_start);

    Char* asm_code = str_new("");
    asm_code = str_cat(asm_code, data_section);
    asm_code = str_cat(asm_code, bss_section);
    asm_code = str_cat(asm_code, text_section);
    asm_code = str_cat(asm_code, start_section);

    // Free temporary strings
    free(data_section);
    free(bss_section);
    free(text_section);
    free(start_section);

    return asm_code;
}

Char* generate(const List* node_list)
{
    printf("Generating assembly...\n");
    Generator* generator = generator_new(node_list);

    // initialize asm instructions
    ininitalize_asm_instructions(generator);

    // Generate assembly code for each node
    for (I32 i = 0; i < node_list->size; i++) {
        const StmtNode* node = (const StmtNode*)node_list->items[i];

        switch (node->type) {
            case NODE_VAR_DECL:
                generate_var_decl_node(generator, &node->var_decl);
                break;
            default:
                UNREACHABLE();
        }
    }

    // Finalize assembly code
    finalize_asm_instructions(generator);

    Char* asm_code = convert_instructionlist_to_string(generator);

    generator_free(generator);
    return asm_code;
}
