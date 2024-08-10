#include "compiler.h"
#include <stdio.h>  // sprintf()
#include <stdlib.h> // calloc()
#include <string.h> // strcmp()

typedef struct AsmG {
    const List* node_list;
    Dict* variables; // associate variable names with their stack_offset
    I32 rbp_offset;  // store the current var offset
    Str* var_prefix; // store the current function name so create asmg->var_prefix when working with variables
    Str* data;
    Str* bss;
    Str* text;
} AsmG;

// Function prototypes
static Void asme_expr(AsmG* asmg, const ExprNode* expr_node);
static Void asme_stmt(AsmG* asmg, const StmtNode* stmt_node);

static AsmG* asmg_new(const List* node_list)
{
    AsmG* asmg = calloc(1, sizeof(AsmG));
    asmg->node_list = node_list;
    asmg->variables = dict_new();
    asmg->rbp_offset = 0;
    asmg->var_prefix = str_new("");
    asmg->data = str_new("");
    asmg->bss = str_new("");
    asmg->text = str_new("");

    return asmg;
}

static Void asmg_free(AsmG* asmg)
{
    dict_free(asmg->variables);
    str_free(asmg->data);
    str_free(asmg->bss);
    str_free(asmg->text);
    free(asmg);
}

static Void asme_num(AsmG* asmg, const NumNode* num_node)
{
    str_cat(asmg->text, "    movq    $");
    str_cat(asmg->text, num_node->value);
    str_cat(asmg->text, ", %rax\n");
}

static Void asme_varref(AsmG* asmg, const VarrefNode* varref_node)
{

    // combine function name and variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varref_node->name);

    I32 var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    if (var_pos == -1) {
        print(MSG_ERROR, "you want to reference %s, but this variable is not defined", str_to_char(full_var_name));
        exit(EXIT_FAILURE);
    }

    Char var_pos_c[256];
    sprintf(var_pos_c, "%d(%%rbp)", var_pos);

    str_cat(asmg->text, "    movq    ");
    str_cat(asmg->text, var_pos_c);
    str_cat(asmg->text, ", %rax\n");
}

static Void asme_varaddr(AsmG* asmg, const VaraddrNode* varaddr_node)
{
    // combine function name and variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varaddr_node->name);

    I32 var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    if (var_pos == -1) {
        print(MSG_ERROR, "you want to get the address of %s, but this variable is not defined", str_to_char(full_var_name));
        exit(EXIT_FAILURE);
    }

    Char var_pos_c[256];
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

static Void asme_unarop(AsmG* asmg, const UnaropNode* unarop_node)
{
    if (unarop_node->op == TOKEN_MINUS) {
        str_cat(asmg->text, "    movq    $");
        str_cat(asmg->text, "-"); // hardcoded but it is ok for now
        str_cat(asmg->text, unarop_node->operand->num_node.value);
        str_cat(asmg->text, ", %rax\n");
    }
    else {
        PANIC("We are only able to convert into asm minus symbol unarop node");
    }
}

static Void asme_funcall(AsmG* asmg, const FuncallNode* funcall_node)
{
    // handle arguments
    I32 num_args = funcall_node->expr_node_list->size;

    // process the first 6 arguments, mapping them to registers
    const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (I32 i = 0; i < num_args && i < 6; i++) {
        const ExprNode* arg_expr = (ExprNode*)funcall_node->expr_node_list->items[i];
        asme_expr(asmg, arg_expr); // the result of the expression should be in %rax
        str_cat(asmg->text, "    movq    %rax, ");
        str_cat(asmg->text, reg_names[i]);
        str_cat(asmg->text, "\n");
    }

    // call the function
    str_cat(asmg->text, "    call    ");
    str_cat(asmg->text, funcall_node->name);
    str_cat(asmg->text, "\n");
}

static Void asme_vardef(AsmG* asmg, const VarDefNode* vardef_node)
{
    if (dev_mode) {
        str_cat(asmg->text, "\n    # variables declaration\n");
    }

    asme_expr(asmg, vardef_node->value); // the value of the variable will be in rax

    // put var in the variable dict
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, vardef_node->name);
    dict_put(asmg->variables, str_to_char(full_var_name), asmg->rbp_offset);

    print(MSG_INFO, "add in the dict: \"%s\" at location %d\n", str_to_char(full_var_name), asmg->rbp_offset);

    Char var_location[256];
    sprintf(var_location, "%d(%%rbp)", asmg->rbp_offset);

    str_cat(asmg->text, "    movq    %rax, ");
    str_cat(asmg->text, var_location);
    str_cat(asmg->text, "\n");

    asmg->rbp_offset -= 8;
}

static Void asme_varmod(AsmG* asmg, const VarModifNode* varmod_node)
{
    if (dev_mode) {
        str_cat(asmg->text, "\n    # variables modification\n");
    }

    asme_expr(asmg, varmod_node->value); // the value of the variable will be in rax

    // combine function name and variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varmod_node->name);

    I32 var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    if (var_pos == -1) {
        print(MSG_ERROR, "you want to modify %s, but this variable is not defined", str_to_char(full_var_name));
        exit(EXIT_FAILURE);
    }

    Char var_pos_c[256];
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
        asme_expr(asmg, return_node->expr_node); // this will put the value into rax
    }

    str_cat(asmg->text, "    movq    %rbp, %rsp\n");
    str_cat(asmg->text, "    pop     %rbp\n");
    str_cat(asmg->text, "    ret\n"); // we can use rax after to use the return value
}

static Void asme_syscall(AsmG* asmg, const SyscallNode* syscall_node)
{
    if (dev_mode) {
        str_cat(asmg->text, "    # syscall\n");
    }
    // handle arguments
    I32 num_args = syscall_node->expr_node_list->size;

    // process the first 6 arguments, mapping them to registers
    const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (I32 i = 0; i < num_args && i < 6; i++) {
        const ExprNode* arg_expr = (ExprNode*)syscall_node->expr_node_list->items[i + 1];
        asme_expr(asmg, arg_expr); // the result of the expression should be in %rax
        str_cat(asmg->text, "    movq    %rax, ");
        str_cat(asmg->text, reg_names[i]);
        str_cat(asmg->text, "\n");
    }

    // process rax argument
    const ExprNode* arg_expr = (ExprNode*)syscall_node->expr_node_list->items[0];
    asme_expr(asmg, arg_expr); // the result of the expression should be in %rax

    // Clean stack if the syscall is a syscall exit
    // TOFIX: Will not work as expected if the error_code is not a number node
    if (strcmp(((ExprNode*)syscall_node->expr_node_list->items[0])->num_node.value, "60") == 0) {
        if (dev_mode) {
            str_cat(asmg->text, "\n    # clean stack\n");
        }
        str_cat(asmg->text, "    movq    %rbp, %rsp\n");
        str_cat(asmg->text, "    pop     %rbp\n");
    }

    // call the function
    str_cat(asmg->text, "    syscall\n");
}

static Void asme_fundef(AsmG* asmg, const FundefNode* fundef_node)
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

    str_cat(asmg->text, "    subq    $88, %rsp\n"); // HARDCODED FOR NOW, you can store 11 variables max in each function

    // pushing arguments onto the stack frame
    if (dev_mode) {
        str_cat(asmg->text, "    # storing arguments into stackframe\n");
    }

    asmg->rbp_offset = -8; // TODO: why not 0 ?

    for (I32 i = 0; i < fundef_node->param_node_list->size; i++) {
        const ParamNode* param_node = (ParamNode*)fundef_node->param_node_list->items[i];

        // put var in the variable dict
        Str* full_var_name = str_new(fundef_node->name);
        str_cat(full_var_name, "_");
        str_cat(full_var_name, param_node->name);
        dict_put(asmg->variables, str_to_char(full_var_name), asmg->rbp_offset);
        print(MSG_INFO, "add in the dict: \"%s\" at location %d\n", str_to_char(full_var_name), asmg->rbp_offset);

        // move the arguments from the registers (rdi, rsi, rdx, rcx, r8, r9) to the stack
        switch (i) {
            case 0:
                str_cat(asmg->text, "    movq    %rdi, ");
                break;
            case 1:
                str_cat(asmg->text, "    movq    %rsi, ");
                break;
            case 2:
                str_cat(asmg->text, "    movq    %rdx, ");
                break;
            case 3:
                str_cat(asmg->text, "    movq    %rcx, ");
                break;
            case 4:
                str_cat(asmg->text, "    movq    %r8, ");
                break;
            case 5:
                str_cat(asmg->text, "    movq    %r9, ");
                break;
            default:
                PANIC("Too many arguments");
        }
        Char arg_pos[20];
        sprintf(arg_pos, "%d(%%rbp)", asmg->rbp_offset);
        str_cat(asmg->text, arg_pos);
        str_cat(asmg->text, "\n");

        // decrease the offset for the next argument
        asmg->rbp_offset -= 8;
    }

    // put the function name in var_prefix
    str_free(asmg->var_prefix);
    asmg->var_prefix = str_new(fundef_node->name);

    // generate code for each stmt of the codeblock_node
    for (I32 i = 0; i < fundef_node->codeblock_node->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)fundef_node->codeblock_node->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }
}

static Void asme_start(AsmG* asmg, const StartdefNode* startdef_node)
{
    str_cat(asmg->text, "\n_start:\n");

    // function prologue
    if (dev_mode) {
        str_cat(asmg->text, "    # function prelude\n");
    }
    str_cat(asmg->text, "    pushq   %rbp\n");
    str_cat(asmg->text, "    movq    %rsp, %rbp\n");

    str_cat(asmg->text, "    subq    $88, %rsp\n"); // you can store 11 variables in each function

    asmg->rbp_offset = -8; // TODO: why not 0 ?

    // put the function name in var_prefix
    str_free(asmg->var_prefix);
    asmg->var_prefix = str_new("_start");

    // generate code for each stmt of the codeblock_node
    for (I32 i = 0; i < startdef_node->codeblock_node->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)startdef_node->codeblock_node->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }
}

static Void asme_expr(AsmG* asmg, const ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_NUMBER:
            asme_num(asmg, &expr_node->num_node);
            break;

        case NODE_VAR_REF:
            asme_varref(asmg, &expr_node->varref_node);
            break;

        case NODE_VAR_ADDR:
            asme_varaddr(asmg, &expr_node->varaddr_node);
            break;

        case NODE_BINOP:
            asme_binop(asmg, &expr_node->binop_node);
            break;

        case NODE_UNARYOP:
            asme_unarop(asmg, &expr_node->unarop_node);
            break;

        case NODE_FUN_CALL:
            asme_funcall(asmg, &expr_node->funcall_node);
            break;

        default:
            UNREACHABLE();
    }
}

static Void asme_stmt(AsmG* asmg, const StmtNode* stmt_node)
{
    switch (stmt_node->type) {
        case NODE_VARDEF:
            asme_vardef(asmg, &stmt_node->vardef_node);
            break;

        case NODE_VARMOD:
            asme_varmod(asmg, &stmt_node->varmod_node);
            break;

        case NODE_RETURN:
            asme_return(asmg, &stmt_node->return_node);
            break;

        case NODE_SYSCALL:
            asme_syscall(asmg, &stmt_node->syscall_node);
            break;

        case NODE_EXPR:
            asme_expr(asmg, &stmt_node->expr_node);
            break;

        default:
            UNREACHABLE();
    }
}

static Void asme_def(AsmG* asmg, const DefNode* def_node)
{
    switch (def_node->type) {
        case NODE_FUNDEF:
            asme_fundef(asmg, &def_node->fundef_node);
            break;

        case NODE_STARTDEF:
            asme_start(asmg, &def_node->startdef_node);
            break;

        default:
            UNREACHABLE();
    }
}

static void init_asm_file(AsmG* asmg)
{
    // Setup initial stmtuctions
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

Str* asme(const List* node_list)
{
    print(MSG_STEP, "generating assembly...\n");
    AsmG* asmg = asmg_new(node_list);

    // initialize asm stmtuctions
    init_asm_file(asmg);

    // Generate assembly code for each node
    for (I32 i = 0; i < node_list->size; i++) {
        const DefNode* def_node = (DefNode*)node_list->items[i];
        asme_def(asmg, def_node);
    }

    // concatenate all code into one string
    Str* asm_code = str_new("");
    str_cat_str(asm_code, asmg->data);
    str_cat_str(asm_code, asmg->bss);
    str_cat_str(asm_code, asmg->text);
    str_cat(asm_code, "\n");

    // clean and return
    asmg_free(asmg);
    return asm_code;
}
