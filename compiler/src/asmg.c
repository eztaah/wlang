#include "compiler.h"
#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AsmG {
    const List* node_list;
    Map* variables; // associate variable names with their stack_offset 
    Str* data;
    Str* bss;
    Str* text;
} AsmG;

// Function prototypes
static Void asme_expr(AsmG* asmg, ExprNode* expr_node);
static Void asme_stmt(AsmG* asmg, StmtNode* stmt_node, I32* rbp_offset);

static AsmG* asmg_new(List* node_list)
{
    AsmG* asmg = calloc(1, sizeof(AsmG));
    asmg->node_list = node_list;
    asmg->variables = map_new();
    asmg->data = str_new("");
    asmg->bss = str_new("");
    asmg->text = str_new("");

    return asmg;
}

static Void asmg_free(AsmG* asmg)
{
    map_free(asmg->variables);
    str_free(asmg->data);
    str_free(asmg->bss);
    str_free(asmg->text);
    free(asmg);
}

static Void asme_num(AsmG* asmg, const NumberNode* num_node)
{
    str_cat(asmg->text, "    movq    $");
    str_cat(asmg->text, num_node->value);
    str_cat(asmg->text, ", %rax\n");
}

static Void asme_varref(AsmG* asmg, const VarRefNode* varref_node)
{
    I32 var_pos = map_get(asmg->variables, varref_node->name);
    if (var_pos == -100) {
        printf("you reference %s, but this variable is not defined", varref_node->name);
        exit(EXIT_FAILURE);
    }

    Char var_pos_c[20];
    sprintf(var_pos_c, "%d(%%rbp)", var_pos);

    str_cat(asmg->text, "    movq    ");
    str_cat(asmg->text, var_pos_c);
    str_cat(asmg->text, ", %rax\n");
}

static Void asme_varaddr(AsmG* asmg, const VarAddrNode* varaddr_node)
{
    I32 var_pos = map_get(asmg->variables, varaddr_node->name);
    if (var_pos == -100) {
        printf("you want to get the address of %s, but this variable is not defined", varaddr_node->name);
        exit(EXIT_FAILURE);
    }

    Char var_pos_c[20];
    sprintf(var_pos_c, "%d(%%rbp)", var_pos);

    str_cat(asmg->text, "    leaq   ");
    str_cat(asmg->text, var_pos_c);
    str_cat(asmg->text, ", %rax\n");
}

static void asme_binop(AsmG* asmg, const BinopNode* binop_node)
{
    if (dev_mode) {
        str_cat(asmg->text, "    # < binop\n");
    }

    asme_expr(asmg, binop_node->right);
    str_cat(asmg->text, "    pushq   %rax\n");
    asme_expr(asmg, binop_node->left);
    str_cat(asmg->text, "    pop     %rbx\n");

    switch (binop_node->op) {
        case TOKEN_PLUS:
            str_cat(asmg->text, "    add     %rbx, %rax\n");
            break;
        case TOKEN_MINUS:
            str_cat(asmg->text, "    sub     %rbx, %rax\n");
            break;
        case TOKEN_MUL:
            str_cat(asmg->text, "    imul    %rbx, %rax\n");
            break;
        case TOKEN_DIV:
            str_cat(asmg->text, "    cqo\n");
            str_cat(asmg->text, "    idiv    %rbx\n");
            break;
        default:
            UNREACHABLE();
    }

    if (dev_mode) {
        str_cat(asmg->text, "    # binop >\n");
    }
}

static Void asme_unarop(AsmG* asmg, const UnaryOpNode* unarop_node)
{
    if (unarop_node->op == TOKEN_MINUS) {
        str_cat(asmg->text, "    movq    $");
        str_cat(asmg->text, "-");                                       // hardcoded but it is ok for now
        str_cat(asmg->text, unarop_node->operand->number_node.value);
        str_cat(asmg->text, ", %rax\n");
    }
    else {
        PANIC("We are only able to convert into asm minus symbol unaryop node");
    }
}

static Void asme_funcall(AsmG* asmg, const FunCallNode* funcall_node)
{
    // handle arguments
    I32 num_args = funcall_node->expr_node_list->size;

    // process the first 6 arguments, mapping them to registers
    const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (I32 i = 0; i < num_args && i < 6; i++) {
        ExprNode* arg_expr = (ExprNode*)funcall_node->expr_node_list->items[i];
        asme_expr(asmg, arg_expr);  // the result of the expression should be in %rax
        str_cat(asmg->text, "    movq    %rax, ");
        str_cat(asmg->text, reg_names[i]);
        str_cat(asmg->text, "\n");
    }

    // call the function
    str_cat(asmg->text, "    call    ");
    str_cat(asmg->text, funcall_node->name);
    str_cat(asmg->text, "\n");
}

static Void asme_vardef(AsmG* asmg, const VarDefNode* vardef_node, I32* rbp_offset)
{
    if (dev_mode) {
        str_cat(asmg->text, "\n    # variables declaration\n");
    }

    asme_expr(asmg, vardef_node->value);   // the value of the variable will be in rax

    map_put(asmg->variables, vardef_node->name, *rbp_offset);

    Char var_location[20];
    sprintf(var_location, "%d(%%rbp)", *rbp_offset);

    str_cat(asmg->text, "    movq    %rax, ");
    str_cat(asmg->text, var_location);
    str_cat(asmg->text, "\n");

    *rbp_offset -= 8;
}

static Void asme_varmod(AsmG* asmg, const VarModifNode* varmod_node)
{
    if (dev_mode) {
        str_cat(asmg->text, "\n    # variables modification\n");
    }

    asme_expr(asmg, varmod_node->value);   // the value of the variable will be in rax

    I32 var_pos = map_get(asmg->variables, varmod_node->name);
    if (var_pos == -100) {
        printf("you want to modify %s, but this variable is not defined", varmod_node->name);
        exit(EXIT_FAILURE);
    }

    Char var_pos_c[20];
    sprintf(var_pos_c, "%d(%%rbp)", var_pos);

    str_cat(asmg->text, "    movq    %rax, ");
    str_cat(asmg->text, var_pos_c);
    str_cat(asmg->text, "\n");
}

static Void asme_return(AsmG* asmg, const ReturnNode* return_node)
{
    if (dev_mode) {
        str_cat(asmg->text, "\n    # return statement\n");
    }

    if (!return_node->is_empty) {
        asme_expr(asmg, return_node->expr_node);    // this will put the value into rax
    }

    str_cat(asmg->text, "    movq    %rbp, %rsp\n");
    str_cat(asmg->text, "    pop     %rbp\n");
    str_cat(asmg->text, "    ret\n");          // we can use rax after to use the return value
}

static Void asme_syscall(AsmG* asmg, const SyscallNode* syscall_node)
{
    // handle arguments
    I32 num_args = syscall_node->expr_node_list->size;

    // process the first 6 arguments, mapping them to registers
    const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (I32 i = 0; i < num_args && i < 0; i++) {
        ExprNode* arg_expr = (ExprNode*)syscall_node->expr_node_list->items[i+1];
        asme_expr(asmg, arg_expr);  // the result of the expression should be in %rax
        str_cat(asmg->text, "    movq    %rax, ");
        str_cat(asmg->text, reg_names[i]);
        str_cat(asmg->text, "\n");
    }

    // process rax argument 
    ExprNode* arg_expr = (ExprNode*)syscall_node->expr_node_list->items[0];
    asme_expr(asmg, arg_expr);  // the result of the expression should be in %rax

    // call the function
    str_cat(asmg->text, "    syscall    ");
}

static Void asme_fundef(AsmG* asmg, const FunDefNode* fundef_node)
{
    // function label
    str_cat(asmg->text, "\n");
    str_cat(asmg->text, fundef_node->name);
    str_cat(asmg->text, ":\n");

    // function prologue
    if (dev_mode) {
        str_cat(asmg->text, "    # function prelude\n");
    }
    str_cat(asmg->text, "    pushq   %rbp\n");
    str_cat(asmg->text, "    movq    %rsp, %rbp\n");

    str_cat(asmg->text, "    subq    $88, %rsp\n");       // you can store 11 variables in each function

    // empilement des arguments dans la stack frame
    if (dev_mode) {
        str_cat(asmg->text, "    # storing arguments into stackframe\n");
    }
    I32 rbp_offset = -8;  // TODO: Pourquoi pas 0 ? 
    for (I32 i = 0; i < fundef_node->param_node_list->size; i++) {
        ParamNode* param_node = (ParamNode*)fundef_node->param_node_list->items[i];

        Char arg_pos[20];
        sprintf(arg_pos, "%d(%%rbp)", rbp_offset);

        // associer le nom de l'argument à son offset dans la pile
        map_put(asmg->variables, param_node->name, rbp_offset);

        // deplacer l'argument des registres (rdi, rsi, rdx, rcx, r8, r9) vers la pile
        switch (i) {
            case 0: str_cat(asmg->text, "    movq    %rdi, "); break;
            case 1: str_cat(asmg->text, "    movq    %rsi, "); break;
            case 2: str_cat(asmg->text, "    movq    %rdx, "); break;
            case 3: str_cat(asmg->text, "    movq    %rcx, "); break;
            case 4: str_cat(asmg->text, "    movq    %r8, "); break;
            case 5: str_cat(asmg->text, "    movq    %r9, "); break;
            default: PANIC("Too many arguments");
        }
        str_cat(asmg->text, arg_pos);
        str_cat(asmg->text, "\n");

        // reduire l'offset pour le prochain argument
        rbp_offset -= 8;
    }

    // generation du code pour chaque instruction du code block
    for (I32 i = 0; i < fundef_node->code_block->stmt_node_list->size; i++) {
        StmtNode* stmt_node = (StmtNode*)fundef_node->code_block->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node, &rbp_offset);
    }
}

static Void asme_start(AsmG* asmg, const StartNode* start_node)
{
    str_cat(asmg->text, "\n_start:\n");

    // function prologue
    if (dev_mode) {
        str_cat(asmg->text, "    # function prelude\n");
    }
    str_cat(asmg->text, "    pushq   %rbp\n");
    str_cat(asmg->text, "    movq    %rsp, %rbp\n");

    str_cat(asmg->text, "    subq    $88, %rsp\n");       // you can store 11 variables in each function

    I32 rbp_offset = -8;  // TODO: Pourquoi pas 0 ? 

    // generation du code pour chaque instruction du code block
    for (I32 i = 0; i < start_node->code_block->stmt_node_list->size; i++) {
        StmtNode* stmt_node = (StmtNode*)start_node->code_block->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node, &rbp_offset);
    }

    // if (dev_mode) {
    //     str_cat(asmg->text, "\n    # function prologue\n");
    // }
    // str_cat(asmg->text, "    movq    %rbp, %rsp\n");
    // str_cat(asmg->text, "    pop     %rbp\n");
}

static Void asme_expr(AsmG* asmg, ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_NUMBER:
            asme_num(asmg, &expr_node->number_node);
            break;

        case NODE_VAR_REF:
            asme_varref(asmg, &expr_node->var_ref_node);
            break;

        case NODE_VAR_ADDR:
            asme_varaddr(asmg, &expr_node->var_addr_node);
            break;

        case NODE_BINOP:
            asme_binop(asmg, &expr_node->bin_op_node);
            break;

        case NODE_UNARYOP:
            asme_unarop(asmg, &expr_node->unary_op_node);
            break;

        case NODE_FUN_CALL:
            asme_funcall(asmg, &expr_node->fun_call_node);
            break;

        default:
            UNREACHABLE();
    }
}

static Void asme_instr(AsmG* asmg, InstrNode* instr_node, I32* rbp_offset)
{
    switch (instr_node->type) {
        case NODE_VAR_DEF:
            asme_vardef(asmg, &instr_node->var_def, rbp_offset);
            break;

        case NODE_VAR_MODIF:
            asme_varmod(asmg, &instr_node->var_modif);
            break;

        case NODE_RETURN:
            asme_return(asmg, &instr_node->return_node);
            break;

        case NODE_SYSCALL:
            asme_syscall(asmg, &instr_node->syscall_node);
            break;

         case NODE_EXPR:
            asme_expr(asmg, &instr_node->expr_node);
            break;       

        default:
            UNREACHABLE();
    }
}

static Void asme_stmt(AsmG* asmg, StmtNode* stmt_node, I32* rbp_offset)
{
    switch (stmt_node->type) {
        case NODE_INSTR:
            asme_instr(asmg, &stmt_node->instr_node, rbp_offset);
            break;

        case NODE_FUN_DEF:
            asme_fundef(asmg, &stmt_node->fun_def_node);
            break;

        case NODE_START:
            asme_start(asmg, &stmt_node->start_node);
            break;

        default:
            UNREACHABLE();
    }
}

static void init_asm_file(AsmG* asmg)
{
    // Setup initial instructions
    if (dev_mode) {
        str_cat(asmg->data, "    # --- SECTION DATA ---\n");
    }
    str_cat(asmg->data, "    .section .data\n");

    if (dev_mode) {
        str_cat(asmg->bss, "\n\n    # --- SECTION BSS ---\n");
    }
    str_cat(asmg->bss, "    .section .bss\n");

    if (dev_mode) {
        str_cat(asmg->text, "\n\n    # --- SECTION TEXT ---\n");
    }
    str_cat(asmg->text, "    .section .text\n");
    str_cat(asmg->text, "    .global _start\n");
}

Str* asme(List* node_list)
{
    printf("Generating assembly...\n");
    AsmG* asmg = asmg_new(node_list);

    // initialize asm instructions
    init_asm_file(asmg);

    // Generate assembly code for each node
    I32 rbp_offset = 0;
    for (I32 i = 0; i < node_list->size; i++) {
        StmtNode* stmt_node = (StmtNode*)node_list->items[i];
        asme_stmt(asmg, stmt_node, &rbp_offset);
    }

    // Concatenate all code into one string
    Str* asm_code = str_new("");
    str_cat_str(asm_code, asmg->data);
    str_cat_str(asm_code, asmg->bss);
    str_cat_str(asm_code, asmg->text);

    asmg_free(asmg);
    return asm_code;
}
