#include "compiler.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Generator {
    const List* node_list;
    I32 var_decl_offset;
    List* variables; // List of strings (variable offsets)
    Str* asm_data;
    Str* asm_bss;
    Str* asm_text;
    Str* asm_start;
} Generator;

// Function prototypes
static Void generate_expression_node(Generator* generator, const ExprNode* expr_node);

static Generator* generator_new(const List* node_list)
{
    Generator* generator = calloc(1, sizeof(Generator));
    generator->node_list = node_list;
    generator->variables = list_new(sizeof(Char*));

    generator->asm_data = str_new("");
    generator->asm_bss = str_new("");
    generator->asm_text = str_new("");
    generator->asm_start = str_new("");

    return generator;
}

static Void generator_free(Generator* generator)
{
    free(generator->variables);
    str_free(generator->asm_data);
    str_free(generator->asm_bss);
    str_free(generator->asm_text);
    str_free(generator->asm_start);
    free(generator);
}

static Void generate_number_node(Generator* generator, const NumberNode* nnode)
{
    str_cat(generator->asm_start, "    movq    $");
    str_cat(generator->asm_start, nnode->value);
    str_cat(generator->asm_start, ", %rax\n");
}

static Void generate_binop_node(Generator* generator, const BinopNode* bnode)
{
    if (dev_mode) {
        str_cat(generator->asm_start, "    # < binOp\n");
    }

    generate_expression_node(generator, bnode->right);
    str_cat(generator->asm_start, "    pushq   %rax\n");
    generate_expression_node(generator, bnode->left);
    str_cat(generator->asm_start, "    pop     %rbx\n");

    switch (bnode->op) {
        case TOKEN_PLUS:
            str_cat(generator->asm_start, "    add     %rbx, %rax\n");
            break;
        case TOKEN_MINUS:
            str_cat(generator->asm_start, "    sub     %rbx, %rax\n");
            break;
        case TOKEN_MUL:
            str_cat(generator->asm_start, "    imul    %rbx, %rax\n");
            break;
        case TOKEN_DIV:
            str_cat(generator->asm_start, "    cqo\n");
            str_cat(generator->asm_start, "    idiv    %rbx\n");
            break;
        default:
            UNREACHABLE();
    }

    if (dev_mode) {
        str_cat(generator->asm_start, "    # binOp >\n");
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
        str_cat(generator->asm_start, "\n    # variables declaration\n");
    }
    Char var_offset[20];
    sprintf(var_offset, "%d(%%rbp)", generator->var_decl_offset);
    list_push(generator->variables, var_offset);

    generate_expression_node(generator, vnode->value);

    str_cat(generator->asm_start, "    movq    %rax, ");
    str_cat(generator->asm_start, var_offset);
    str_cat(generator->asm_start, "\n");

    generator->var_decl_offset -= 8;
}


void initialize_asm_instructions(Generator* generator)
{
    // Setup initial instructions
    if (dev_mode) {
        str_cat(generator->asm_data, "    # --- SECTION DATA ---\n");
    }
    str_cat(generator->asm_data, "    .section .data\n");

    if (dev_mode) {
        str_cat(generator->asm_bss, "\n\n    # --- SECTION BSS ---\n");
    }
    str_cat(generator->asm_bss, "    .section .bss\n");

    if (dev_mode) {
        str_cat(generator->asm_text, "\n\n    # --- SECTION TEXT ---\n");
    }
    str_cat(generator->asm_text, "    .section .text\n");
    str_cat(generator->asm_text, "    .global _start\n");

    if (dev_mode) {
        str_cat(generator->asm_start, "\n_start:\n");
    }
    else {
        str_cat(generator->asm_start, "_start:\n");
    }

    if (dev_mode) {
        str_cat(generator->asm_start, "    # initialization\n");
    }

    str_cat(generator->asm_start, "    pushq   %rbp\n");       // save old stack pointer ?
    str_cat(generator->asm_start, "    movq    %rsp, %rbp\n"); // ?????

    // Prépare la pile pour être capable d'accueillir toutes les variables déclarées dans le code
    I32 stack_space = generator->node_list->size * 8;
    if (generator->node_list->size % 2 == 0) {
        stack_space += 8;
    }
    Char stack_space_str[20];
    sprintf(stack_space_str, "%d", stack_space);

    str_cat(generator->asm_start, "    subq    $");
    str_cat(generator->asm_start, stack_space_str);
    str_cat(generator->asm_start, ", %rsp\n");
}


Void finalize_asm_instructions(Generator* generator)
{
    // exit
    if (dev_mode) {
        str_cat(generator->asm_start, "\n.ENDP:\n");
    }
    else {
        str_cat(generator->asm_start, ".ENDP:\n");
    }

    if (dev_mode) {
        str_cat(generator->asm_start, "    # end of the program\n");
    }
    str_cat(generator->asm_start, "    movq    %rbp, %rsp\n");
    str_cat(generator->asm_start, "    pop     %rbp\n");
    if (dev_mode) {
        str_cat(generator->asm_start, "    # exit syscall\n");
    }
    str_cat(generator->asm_start, "    movq    $60, %rax\n");
    str_cat(generator->asm_start, "    xor     %rdi, %rdi\n");
    str_cat(generator->asm_start, "    syscall\n");
}

Str* generate(const List* node_list)
{
    printf("Generating assembly...\n");
    Generator* generator = generator_new(node_list);

    // initialize asm instructions
    initialize_asm_instructions(generator);

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

    // Concatenate all code into one string
    Str* asm_code = str_new("");
    str_cat_str(asm_code, generator->asm_data);
    str_cat_str(asm_code, generator->asm_bss);
    str_cat_str(asm_code, generator->asm_text);
    str_cat_str(asm_code, generator->asm_start);

    generator_free(generator);
    return asm_code;
}
