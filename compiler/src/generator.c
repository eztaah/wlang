#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration of Generator structure
typedef struct Generator Generator;

// Function prototypes
static Void generate_expression_node(Generator* generator, const ExpressionNode* expr_node);

typedef struct Generator {
    const List* node_list;
    I32 index;
    Char* output;
    I32 offset;
    I32 string_label_count;
    I32 global_label_count;
    List* data_instructions;
    List* bss_instructions;
    List* text_instructions;
    List* start_function_instructions;
    B32 print_string;
    B32 print_number;
    B32 scanf_is_used;
    List* variables; // List of strings (variable offsets)
} Generator;

static Generator* instanciate_generator(const List* node_list)
{
    Generator* generator = calloc(1, sizeof(Generator));
    generator->node_list = node_list;
    generator->index = 0;
    generator->output = init_empty_string();
    generator->offset = 0;
    generator->string_label_count = 0;
    generator->global_label_count = 0;
    generator->data_instructions = init_list(sizeof(Char*));
    generator->bss_instructions = init_list(sizeof(Char*));
    generator->text_instructions = init_list(sizeof(Char*));
    generator->start_function_instructions = init_list(sizeof(Char*));
    generator->print_string = false;
    generator->print_number = false;
    generator->scanf_is_used = false;
    generator->variables = init_list(sizeof(Char*));
    return generator;
}

static Void destroy_generator(Generator* generator)
{
    free(generator->output);
    free(generator->data_instructions);
    free(generator->bss_instructions);
    free(generator->text_instructions);
    free(generator->start_function_instructions);
    free(generator->variables);
    free(generator);
}

static Char* concat_instructions(List* instructions)
{
    Char* result = init_empty_string();
    for (I32 i = 0; i < instructions->size; i++) {
        result = strcat_improved(result, (Char*)instructions->items[i]);
        result = strcat_improved(result, "\n");
    }
    return result;
}

static Void generate_number_node(Generator* generator, const NumberNode* nnode)
{
    list_push(generator->start_function_instructions, init_string_with_value("    movq    $"));
    list_push(generator->start_function_instructions, nnode->value);
    list_push(generator->start_function_instructions, init_string_with_value(", %rax"));
}

static Void generate_binop_node(Generator* generator, const BinOpNode* bnode)
{
    generate_expression_node(generator, bnode->right);
    list_push(generator->start_function_instructions, init_string_with_value("    pushq   %rax"));
    generate_expression_node(generator, bnode->left);
    list_push(generator->start_function_instructions, init_string_with_value("    pop     %rbx"));

    switch (bnode->op) {
        case TOKEN_PLUS:
            list_push(generator->start_function_instructions, init_string_with_value("    add     %rbx, %rax"));
            break;
        case TOKEN_MINUS:
            list_push(generator->start_function_instructions, init_string_with_value("    sub     %rbx, %rax"));
            break;
        case TOKEN_MUL:
            list_push(generator->start_function_instructions, init_string_with_value("    imul    %rbx, %rax"));
            break;
        case TOKEN_DIV:
            list_push(generator->start_function_instructions, init_string_with_value("    cqo"));
            list_push(generator->start_function_instructions, init_string_with_value("    idiv    %rbx"));
            break;
        default:
            UNREACHABLE();
    }
}

static Void generate_expression_node(Generator* generator, const ExpressionNode* expr_node)
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
    Char* var_offset = malloc(20);
    sprintf(var_offset, "%d(%%rbp)", generator->offset);
    list_push(generator->variables, var_offset);

    generate_expression_node(generator, vnode->value);
    list_push(generator->start_function_instructions, init_string_with_value("    movq    %rax, "));
    list_push(generator->start_function_instructions, var_offset);

    generator->offset -= 8;
}

static Void generate_internal(Generator* generator, const StatementNode* node)
{
    switch (node->type) {
        case NODE_VAR_DECL:
            generate_var_decl_node(generator, &node->var_decl);
            break;
        default:
            UNREACHABLE();
    }
}

Char* generate(const List* node_list)
{
    printf("Generating assembly...\n");

    Generator* generator = instanciate_generator(node_list);

    // Setup initial instructions
    list_push(generator->data_instructions, init_string_with_value("    .section .data"));
    list_push(generator->bss_instructions, init_string_with_value("    .section .bss"));
    list_push(generator->text_instructions, init_string_with_value("    .section .text"));
    list_push(generator->text_instructions, init_string_with_value("    .global _start"));
    list_push(generator->start_function_instructions, init_string_with_value("_start:"));
    list_push(generator->start_function_instructions, init_string_with_value("    pushq   %rbp"));         // save old stack pointer ?
    list_push(generator->start_function_instructions, init_string_with_value("    movq    %rsp, %rbp"));   // ?????

    // Pépare la pile pour etre capable d'acceuillir toutes les variables déclarées dans le code
    I32 stack_space = node_list->size * 8;
    if (node_list->size % 2 == 0) {
        stack_space += 8;
    }
    Char stack_space_str[20];
    sprintf(stack_space_str, "%d", stack_space);
    list_push(generator->start_function_instructions, init_string_with_value("    subq    $"));
    list_push(generator->start_function_instructions, init_string_with_value(stack_space_str));
    list_push(generator->start_function_instructions, init_string_with_value(", %rsp"));

    // Generate assembly code for each node
    for (I32 i = 0; i < node_list->size; i++) {
        const StatementNode* node = (const StatementNode*)node_list->items[i];
        generate_internal(generator, node);
    }

    // Finalize assembly code
    list_push(generator->start_function_instructions, init_string_with_value("    movq    %rbp, %rsp"));
    list_push(generator->start_function_instructions, init_string_with_value("    popq    %rbp"));
    list_push(generator->start_function_instructions, init_string_with_value("    movq    $60, %rax"));
    list_push(generator->start_function_instructions, init_string_with_value("    xor     %rdi, %rdi"));
    list_push(generator->start_function_instructions, init_string_with_value("    syscall"));

    // Combine all instructions into a single output string
    Char* data_section = concat_instructions(generator->data_instructions);
    Char* bss_section = concat_instructions(generator->bss_instructions);
    Char* text_section = concat_instructions(generator->text_instructions);
    Char* start_section = concat_instructions(generator->start_function_instructions);
    generator->output = strcat_improved(generator->output, data_section);
    generator->output = strcat_improved(generator->output, bss_section);
    generator->output = strcat_improved(generator->output, text_section);
    generator->output = strcat_improved(generator->output, start_section);

    // Free temporary strings
    free(data_section);
    free(bss_section);
    free(text_section);
    free(start_section);

    Char* result = strdup(generator->output);
    destroy_generator(generator);
    return result;
}
