#include <stdio.h>  // sprintf()
#include <stdlib.h> // safe_calloc()
#include <string.h> // strcmp()

#include "compiler.h"

typedef struct {
    List* loop_end_labels;
} LoopLabelStack;

static LoopLabelStack* loop_label_stack_new()
{
    LoopLabelStack* stack = safe_calloc(1, sizeof(LoopLabelStack));
    stack->loop_end_labels = list_new(sizeof(char*));
    return stack;
}

static void loop_label_stack_free(LoopLabelStack* stack)
{
    for (int i = 0; i < stack->loop_end_labels->size; i++) {
        free(stack->loop_end_labels->items[i]);
    }
    list_free(stack->loop_end_labels);
    free(stack);
}

static void loop_label_stack_push(LoopLabelStack* stack, const char* label)
{
    list_push(stack->loop_end_labels, strdup(label));
}

static char* loop_label_stack_pop(LoopLabelStack* stack)
{
    return list_pop(stack->loop_end_labels);
}

static char* loop_label_stack_top(LoopLabelStack* stack)
{
    return (char*)stack->loop_end_labels->items[stack->loop_end_labels->size - 1];
}

typedef struct AsmG {
    const List* node_list;
    Dict* variables; // associate variable names with their stack_offset as a string
    Dict* arrays;
    int rbp_offset;  // store the current var offset
    Str* var_prefix; // store the current function name so create asmg->var_prefix when working with variables
    Str* data;
    Str* bss;
    Str* text_header;
    Str* global;
    Str* start;
    Str* text;

    Str* fun_body;
    Str* fun_prol;
    LoopLabelStack* loop_stack; // Stack to manage loop end labels
} AsmG;

// Function prototypes
static void asme_expr(AsmG* asmg, const ExprNode* expr_node);
static void asme_stmt(AsmG* asmg, const StmtNode* stmt_node);

static AsmG* asmg_new(const List* node_list)
{
    AsmG* asmg = safe_calloc(1, sizeof(AsmG));
    asmg->node_list = node_list;
    asmg->variables = dict_new();
    asmg->arrays = dict_new();
    asmg->rbp_offset = 0;
    asmg->var_prefix = str_new("");
    asmg->data = str_new("");
    asmg->bss = str_new("");
    asmg->text = str_new("");
    asmg->text_header = str_new("");
    asmg->global = str_new("");
    asmg->start = str_new("");
    asmg->fun_body = str_new("");
    asmg->fun_prol = str_new("");
    asmg->loop_stack = loop_label_stack_new();

    return asmg;
}

static void asmg_free(AsmG* asmg)
{
    dict_free(asmg->variables);
    dict_free(asmg->arrays);
    str_free(asmg->data);
    str_free(asmg->bss);
    str_free(asmg->text_header);
    str_free(asmg->text);
    str_free(asmg->fun_prol);
    str_free(asmg->fun_body);
    str_free(asmg->global);
    str_free(asmg->start);
    loop_label_stack_free(asmg->loop_stack);
    free(asmg);
}

static void asme_num(AsmG* asmg, const NumNode* num_node)
{
    // load the immediate value into the %rax register
    str_cat(asmg->fun_body, "    movq    $");
    str_cat(asmg->fun_body, num_node->value);
    str_cat(asmg->fun_body, ", %rax\n");
}

static void asme_varref(AsmG* asmg, const VarrefNode* varref_node)
{
    // construct the full variable name by combining the function prefix with the variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varref_node->name);

    // retrieve the memory offset of the variable from the stack frame
    char* var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    char* array_pos = dict_get(asmg->arrays, str_to_char(full_var_name));

    if (!var_pos && !array_pos) {
        PANIC("reference to %s, but this variable is not defined (should be caught by the semantic analyser)", str_to_char(full_var_name));
    }

    if (var_pos) {
        // for normal variables, move the value at the stack offset into %rax
        str_cat(asmg->fun_body, "    movq    ");
        str_cat(asmg->fun_body, var_pos);
        str_cat(asmg->fun_body, ", %rax\n");
    }
    else if (array_pos) {
        // for arrays, load the base address into %rax
        str_cat(asmg->fun_body, "    leaq    ");
        str_cat(asmg->fun_body, array_pos);
        str_cat(asmg->fun_body, ", %rax\n");
    }
}

static void asme_varaddr(AsmG* asmg, const VaraddrNode* varaddr_node)
{
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varaddr_node->name);

    char* var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    char* array_pos = dict_get(asmg->arrays, str_to_char(full_var_name));

    if (!var_pos && !array_pos) {
        PANIC("reference to %s, but this variable is not defined (should be caught by the semantic analyser)", str_to_char(full_var_name));
    }

    // load the address of the variable or the array base into %rax
    str_cat(asmg->fun_body, "    leaq    ");
    str_cat(asmg->fun_body, var_pos);
    str_cat(asmg->fun_body, ", %rax\n");
}

static void asme_l_unarop(AsmG* asmg, const UnaropNode* unarop_node)
{
    // handle dereferencing for the left-hand side of an assignment.
    switch (unarop_node->op) {
        case TOKEN_CARET:
            // dereference the operand and leave its address in %rax.
            asme_expr(asmg, unarop_node->operand);
            break;

        default:
            PANIC("the dereference operator is allowed only on the left side of an assignment");
    }
}

static void asme_binop(AsmG* asmg, const BinopNode* binop_node)
{
    str_cat(asmg->fun_body, "    # < binop\n");

    // evaluate the right operand and push the result onto the stack
    asme_expr(asmg, binop_node->right);
    str_cat(asmg->fun_body, "    pushq   %rax\n");

    // evaluate the left operand and place the result in %rax
    asme_expr(asmg, binop_node->left);
    str_cat(asmg->fun_body, "    popq     %rbx\n"); // pop the right operand into %rbx for use in the operation.

    switch (binop_node->op) {
        case TOKEN_PLUS:
            str_cat(asmg->fun_body, "    add     %rbx, %rax\n");
            break;

        case TOKEN_MINUS:
            str_cat(asmg->fun_body, "    sub     %rbx, %rax\n");
            break;

        case TOKEN_MUL:
            str_cat(asmg->fun_body, "    imul    %rbx, %rax\n");
            break;

        case TOKEN_DIV:
            // use cqo to extend the sign of %rax into %rdx for division
            str_cat(asmg->fun_body, "    cqo\n");
            str_cat(asmg->fun_body, "    idiv    %rbx\n");
            break;

        case TOKEN_PERCENT:
            str_cat(asmg->fun_body, "    cqo\n");
            str_cat(asmg->fun_body, "    idiv    %rbx\n");
            str_cat(asmg->fun_body, "    movq    %rdx, %rax\n"); // the remainder of the division is in %rdx
            break;

        case TOKEN_LEFTSHIFT:
            // shift the value in %rax left by the amount specified in %cl (lower 8 bits of %rcx)
            str_cat(asmg->fun_body, "    movq    %rbx, %rcx\n");
            str_cat(asmg->fun_body, "    shl     %cl, %rax\n");
            break;

        case TOKEN_RIGHTSHIFT:
            // shift the value in %rax right by the amount specified in %cl
            str_cat(asmg->fun_body, "    movq    %rbx, %rcx\n");
            str_cat(asmg->fun_body, "    shr     %cl, %rax\n");
            break;

        case TOKEN_AMPERSAND:
            str_cat(asmg->fun_body, "    and     %rbx, %rax\n");
            break;

        case TOKEN_HASH:
            str_cat(asmg->fun_body, "    xor     %rbx, %rax\n");
            break;

        case TOKEN_PIPE:
            str_cat(asmg->fun_body, "    or      %rbx, %rax\n");
            break;

        case TOKEN_AND:
            str_cat(asmg->fun_body, "    cmpq    $0, %rax\n");
            str_cat(asmg->fun_body, "    setne   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            str_cat(asmg->fun_body, "    cmpq    $0, %rbx\n");
            str_cat(asmg->fun_body, "    setne   %bl\n");
            str_cat(asmg->fun_body, "    movzbq  %bl, %rbx\n");
            str_cat(asmg->fun_body, "    and     %rbx, %rax\n");
            break;

        case TOKEN_OR:
            str_cat(asmg->fun_body, "    cmpq    $0, %rax\n");
            str_cat(asmg->fun_body, "    setne   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            str_cat(asmg->fun_body, "    cmpq    $0, %rbx\n");
            str_cat(asmg->fun_body, "    setne   %bl\n");
            str_cat(asmg->fun_body, "    movzbq  %bl, %rbx\n");
            str_cat(asmg->fun_body, "    or      %rbx, %rax\n");
            break;

        case TOKEN_EQUAL_EQUAL:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    sete    %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            break;

        case TOKEN_NOT_EQUAL:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setne   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            break;

        case TOKEN_LESSTHAN:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setl    %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            break;

        case TOKEN_GREATERTHAN:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setg    %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            break;

        case TOKEN_LESSTHAN_EQ:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setle   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            break;

        case TOKEN_GREATERTHAN_EQ:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setge   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n");
            break;

        default:
            UNREACHABLE();
    }

    str_cat(asmg->fun_body, "    # binop >\n");
}

static void asme_unarop(AsmG* asmg, const UnaropNode* unarop_node)
{
    str_cat(asmg->fun_body, "    # < unarop\n");

    switch (unarop_node->op) {
        case TOKEN_MINUS:
            // negate the operand by computing two's complement
            asme_expr(asmg, unarop_node->operand);
            str_cat(asmg->fun_body, "    neg     %rax\n");
            break;

        case TOKEN_TILDE:
            // bitwise NOT: flip all bits in the operand
            asme_expr(asmg, unarop_node->operand);
            str_cat(asmg->fun_body, "    not     %rax\n");
            break;

        case TOKEN_CARET:
            // dereference: load the value pointed to by %rax into %rax
            asme_expr(asmg, unarop_node->operand);
            str_cat(asmg->fun_body, "    movq    (%rax), %rbx\n");
            str_cat(asmg->fun_body, "    movq    %rbx, %rax\n");
            break;

        default:
            PANIC("unknown unary operator in asm generator");
    }

    str_cat(asmg->fun_body, "    # unarop >\n");
}

static void asme_funcall(AsmG* asmg, const FuncallNode* funcall_node)
{
    str_cat(asmg->fun_body, "\n    # function call\n");

    int num_args = funcall_node->expr_node_list->size;

    // handle arguments beyond the 6th by pushing them onto the stack in reverse order
    for (int i = num_args - 1; i >= 6; i--) {
        const ExprNode* arg_expr = (ExprNode*)funcall_node->expr_node_list->items[i];
        asme_expr(asmg, arg_expr);
        str_cat(asmg->fun_body, "    pushq   %rax\n");
    }

    // map the first 6 arguments to registers
    const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = 0; i < num_args && i < 6; i++) {
        const ExprNode* arg_expr = (ExprNode*)funcall_node->expr_node_list->items[i];
        asme_expr(asmg, arg_expr);
        str_cat(asmg->fun_body, "    movq    %rax, ");
        str_cat(asmg->fun_body, reg_names[i]);
        str_cat(asmg->fun_body, "\n");
    }

    // call the function.
    str_cat(asmg->fun_body, "    call    ");
    str_cat(asmg->fun_body, funcall_node->name);
    str_cat(asmg->fun_body, "\n");

    // clean up the stack if more than 6 arguments were passed
    if (num_args > 6) {
        char stack_cleanup[256];
        sprintf(stack_cleanup, "    addq    $%d, %%rsp\n", 8 * (num_args - 6));
        str_cat(asmg->fun_body, stack_cleanup);
    }
}

static void asme_syscall(AsmG* asmg, const SyscNode* sysc_node)
{
    str_cat(asmg->fun_body, "\n    # syscall\n");

    int num_args = sysc_node->expr_node_list->size;

    // map the first 6 arguments to registers
    const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (int i = 0; i < num_args && i < 6; i++) {
        const ExprNode* arg_expr = (ExprNode*)sysc_node->expr_node_list->items[i + 1];
        asme_expr(asmg, arg_expr);
        str_cat(asmg->fun_body, "    movq    %rax, ");
        str_cat(asmg->fun_body, reg_names[i]);
        str_cat(asmg->fun_body, "\n");
    }

    // handle the syscall number in %rax
    const ExprNode* arg_expr = (ExprNode*)sysc_node->expr_node_list->items[0];
    asme_expr(asmg, arg_expr);

    // special handling for the `exit` syscall (syscall number 60).
    // ensure that the stack is clean before exiting
    if (strcmp(((ExprNode*)sysc_node->expr_node_list->items[0])->num_node.value, "60") == 0) {
        str_cat(asmg->fun_body, "\n    # clean stack (only for exit syscall)\n");
        str_cat(asmg->fun_body, "    movq    %rbp, %rsp\n");
        str_cat(asmg->fun_body, "    popq     %rbp\n");
    }

    // perform the syscall
    str_cat(asmg->fun_body, "    syscall\n");
}

static void asme_lexpr(AsmG* asmg, const ExprNode* expr_node);

static void asme_ass(AsmG* asmg, const AssNode* ass_node)
{
    str_cat(asmg->fun_body, "\n    # variable assignment\n");

    // get the lvalue address and place it in %rax
    asme_lexpr(asmg, ass_node->lvalue);

    // push the address onto the stack
    str_cat(asmg->fun_body, "    pushq   %rax\n");

    // evaluate the right-hand side expression and place the result in %rax
    asme_expr(asmg, ass_node->value);

    // pop the address from the stack into %rbx
    str_cat(asmg->fun_body, "    popq    %rbx\n");

    // store the value in %rax at the address in %rbx
    str_cat(asmg->fun_body, "    movq    %rax, (%rbx)\n");
}

static void asme_if(AsmG* asmg, const IfNode* if_node)
{
    str_cat(asmg->fun_body, "\n    # if statement\n");

    static int if_label_counter = 0; // unique label counter for if statements
    int current_label = if_label_counter++;

    // evaluate the condition
    asme_expr(asmg, if_node->cond_node);
    str_cat(asmg->fun_body, "    cmpq    $0, %rax\n"); // compare the result with 0

    // conditionally jump to the false block if the condition is false
    char label_false[64];
    sprintf(label_false, "    je      .Lelse%d\n", current_label);
    str_cat(asmg->fun_body, label_false);

    // generate code for the true block
    for (int i = 0; i < if_node->true_block->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)if_node->true_block->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }

    // unconditionally jump to the end of the if statement
    char label_end[64];
    sprintf(label_end, "    jmp     .Lend%d\n", current_label);
    str_cat(asmg->fun_body, label_end);

    // generate code for the false block (if it exists)
    str_cat(asmg->fun_body, ".Lelse");
    sprintf(label_false, "%d:\n", current_label);
    str_cat(asmg->fun_body, label_false);
    if (if_node->false_block) {
        for (int i = 0; i < if_node->false_block->stmt_node_list->size; i++) {
            const StmtNode* stmt_node = (StmtNode*)if_node->false_block->stmt_node_list->items[i];
            asme_stmt(asmg, stmt_node);
        }
    }

    // end label for the if statement
    str_cat(asmg->fun_body, ".Lend");
    sprintf(label_end, "%d:\n", current_label);
    str_cat(asmg->fun_body, label_end);
}

static void asme_loop(AsmG* asmg, const LoopNode* loop_node)
{
    str_cat(asmg->fun_body, "\n    # loop\n");

    static int loop_label_counter = 0; // unique label counter for loops
    int current_label = loop_label_counter++;

    // generate labels for the start and end of the loop
    char label_start[64];
    char label_end[64];
    sprintf(label_start, ".Lloop%d", current_label);
    sprintf(label_end, ".Lloopend%d", current_label);

    // push the end label onto the loop stack for handling breaks
    loop_label_stack_push(asmg->loop_stack, label_end);

    // loop start label
    str_cat(asmg->fun_body, label_start);
    str_cat(asmg->fun_body, ":\n");

    // generate code for each statement in the loop body
    for (int i = 0; i < loop_node->block->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)loop_node->block->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }

    // jump back to the loop start for the next iteration
    str_cat(asmg->fun_body, "    jmp     ");
    str_cat(asmg->fun_body, label_start);
    str_cat(asmg->fun_body, "\n");

    // loop end label (used by break statements)
    str_cat(asmg->fun_body, label_end);
    str_cat(asmg->fun_body, ":\n");

    // pop the end label off the loop stack
    loop_label_stack_pop(asmg->loop_stack);
}

static void asme_break(AsmG* asmg)
{
    str_cat(asmg->fun_body, "\n    # break\n");

    // use the top label from the loop stack to jump to the end of the loop
    char* label_end = loop_label_stack_top(asmg->loop_stack);
    str_cat(asmg->fun_body, "    jmp     ");
    str_cat(asmg->fun_body, label_end);
    str_cat(asmg->fun_body, "\n");
}

static void asme_vardec(AsmG* asmg, const VardecNode* vardec_node)
{
    str_cat(asmg->fun_body, "\n    # variable declaration\n");

    // construct the full variable name using the function prefix
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, vardec_node->name);

    // calculate the stack offset for the variable and store it in the dictionary
    char var_pos_c[256];
    sprintf(var_pos_c, "%d(%%rbp)", asmg->rbp_offset);
    dict_put(asmg->variables, str_to_char(full_var_name), var_pos_c);

    print(VERBOSE, 2, "add in the dict: \"%s\" at location %s\n", str_to_char(full_var_name), var_pos_c);

    // if an initial value is provided, evaluate it and store it at the calculated offset
    if (vardec_node->value != NULL) {
        asme_expr(asmg, vardec_node->value);
        str_cat(asmg->fun_body, "    movq    %rax, ");
        str_cat(asmg->fun_body, var_pos_c);
        str_cat(asmg->fun_body, "\n");
    }

    // update the stack pointer offset for subsequent variables
    asmg->rbp_offset -= 8;
}

static void asme_arraydec(AsmG* asmg, const ArraydecNode* arraydec_node)
{
    str_cat(asmg->fun_body, "\n    # array declaration\n");

    // construct the full array name using the function prefix
    Str* full_array_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_array_name, "_");
    str_cat(full_array_name, arraydec_node->name);

    // calculate the stack offset for the array and store it in the dictionary
    char array_pos_c[256];
    sprintf(array_pos_c, "%d(%%rbp)", asmg->rbp_offset);
    dict_put(asmg->arrays, str_to_char(full_array_name), array_pos_c);

    print(VERBOSE, 2, "add array 1st elmt in the dict: \"%s\" at location %s\n", str_to_char(full_array_name), array_pos_c);

    // convert array size to an integer
    int array_size = atoi(arraydec_node->size);

    // initialize the array elements if an initialization list is provided
    if (arraydec_node->expr_node_list != NULL) {
        int current_offset = asmg->rbp_offset;

        for (int i = 0; i < array_size; i++) {
            const ExprNode* arg_expr = (ExprNode*)arraydec_node->expr_node_list->items[i];
            asme_expr(asmg, arg_expr);

            char var_location[256];
            current_offset -= 8;

            sprintf(var_location, "%d(%%rbp)", current_offset);

            str_cat(asmg->fun_body, "    movq    %rax, ");
            str_cat(asmg->fun_body, var_location);
            str_cat(asmg->fun_body, "\n");
        }
    }

    // adjust the stack pointer to account for the space required by the array
    asmg->rbp_offset -= 8 * (array_size + 1);
}

static void asme_ret(AsmG* asmg, const RetNode* ret_node)
{
    str_cat(asmg->fun_body, "\n    # return statement\n");

    // if the return statement includes an expression, evaluate it and place the result in %rax
    if (ret_node->expr_node) {
        asme_expr(asmg, ret_node->expr_node);
    }

    // jump to the epilogue to clean up and return from the function
    char epilogue_label[64];
    sprintf(epilogue_label, "    jmp     .Lepilogue_%s\n", str_to_char(asmg->var_prefix));
    str_cat(asmg->fun_body, epilogue_label);
}

static void asme_fundef(AsmG* asmg, const FundefNode* fundef_node)
{
    if (strcmp(fundef_node->name, "w__main") == 0) {
        // generate the entry point for the program, which calls the main function
        str_cat(asmg->global, "    .global _start\n");
        str_cat(asmg->start, "\n_start:\n");
        str_cat(asmg->start, "    # function call\n");
        str_cat(asmg->start, "    call w__main\n");
        str_cat(asmg->start, "\n    # syscall\n");
        str_cat(asmg->start, "    movq %rax, %rdi\n");
        str_cat(asmg->start, "    movq $60, %rax\n");
        str_cat(asmg->start, "    syscall\n");
    }

    if (strcmp(fundef_node->scope, "global") == 0) {
        // declare the function as global if specified
        str_cat(asmg->global, "    .global ");
        str_cat(asmg->global, fundef_node->name);
        str_cat(asmg->global, "\n");
    }

    // clear fun_body and fun_prol
    str_free(asmg->fun_prol);
    asmg->fun_prol = str_new("");

    str_free(asmg->fun_body);
    asmg->fun_body = str_new("");

    // generate the function label
    str_cat(asmg->fun_prol, "\n\n");
    str_cat(asmg->fun_prol, fundef_node->name);
    str_cat(asmg->fun_prol, ":\n");

    // function prologue: save %rbp and establish the new stack frame
    str_cat(asmg->fun_prol, "    # prologue\n");
    str_cat(asmg->fun_prol, "    pushq   %rbp\n");
    str_cat(asmg->fun_prol, "    movq    %rsp, %rbp\n");

    // preserve callee-saved registers
    // str_cat(asmg->fun_prol, "    pushq   %rbx\n");
    // str_cat(asmg->fun_prol, "    pushq   %r12\n");
    // str_cat(asmg->fun_prol, "    pushq   %r13\n");
    // str_cat(asmg->fun_prol, "    pushq   %r14\n");
    // str_cat(asmg->fun_prol, "    pushq   %r15\n");

    // retrieve function arguments passed via registers or stack
    if (fundef_node->param_node_list->size != 0) {
        str_cat(asmg->fun_body, "\n    # retrieve arguments\n");
    }

    asmg->rbp_offset = -8; // start the offset for the first local variable

    int num_params = fundef_node->param_node_list->size;

    for (int i = 0; i < num_params; i++) {
        const ParamNode* param_node = (ParamNode*)fundef_node->param_node_list->items[i];

        // fet the full name of the parameter, including the function prefix
        Str* full_var_name = str_new(fundef_node->name);
        str_cat(full_var_name, "_");
        str_cat(full_var_name, param_node->name);

        // calculate the stack offset for the parameter and store it
        char var_pos_c[256];
        sprintf(var_pos_c, "%d(%%rbp)", asmg->rbp_offset);
        dict_put(asmg->variables, str_to_char(full_var_name), var_pos_c);

        print(VERBOSE, 2, "add in the dict: \"%s\" at location %s\n", str_to_char(full_var_name), var_pos_c);

        if (i < 6) {
            // move the argument from the register (e.g., %rdi, %rsi) to the stack
            const char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            str_cat(asmg->fun_body, "    movq    ");
            str_cat(asmg->fun_body, reg_names[i]);
            str_cat(asmg->fun_body, ", ");
            str_cat(asmg->fun_body, var_pos_c);
            str_cat(asmg->fun_body, "\n");
        }
        else {
            // retrieve arguments passed via the stack beyond the first 6
            int stack_offset = (i - 6 + 1) * 8;
            char stack_offset_c[256];
            sprintf(stack_offset_c, "%d(%%rbp)", stack_offset + 8); // 8 for the return address offset.
            str_cat(asmg->fun_body, "    movq    ");
            str_cat(asmg->fun_body, stack_offset_c);
            str_cat(asmg->fun_body, ", %rax\n");
            str_cat(asmg->fun_body, "    movq    %rax, ");
            str_cat(asmg->fun_body, var_pos_c);
            str_cat(asmg->fun_body, "\n");
        }

        // decrease the offset for the next local variable
        asmg->rbp_offset -= 8;
    }

    // store the function name in var_prefix for use in variable naming
    str_free(asmg->var_prefix);
    asmg->var_prefix = str_new(fundef_node->name);

    // generate assembly code for each statement in the function body
    for (int i = 0; i < fundef_node->block_node->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)fundef_node->block_node->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }

    // calculate the required stack space for local variables, ensuring 16-byte alignment (for system V ABI)
    int required_space = abs(asmg->rbp_offset);
    if (required_space % 16 != 0) {
        required_space += 8;
    }
    char required_space_str[255];
    sprintf(required_space_str, "%d", required_space);
    str_cat(asmg->fun_prol, "    subq    $");
    str_cat(asmg->fun_prol, required_space_str);
    str_cat(asmg->fun_prol, ", %rsp\n");

    // generate the function epilogue label.
    char epilogue_label[64];
    sprintf(epilogue_label, ".Lepilogue_%s:\n", str_to_char(asmg->var_prefix));
    str_cat(asmg->fun_body, epilogue_label);
    str_cat(asmg->fun_body, "\n    # epilogue\n");

    // restore callee-saved registers
    // str_cat(asmg->fun_body, "    popq    %r15\n");
    // str_cat(asmg->fun_body, "    popq    %r14\n");
    // str_cat(asmg->fun_body, "    popq    %r13\n");
    // str_cat(asmg->fun_body, "    popq    %r12\n");
    // str_cat(asmg->fun_body, "    popq    %rbx\n");

    // restore the stack frame and return
    str_cat(asmg->fun_body, "    movq    %rbp, %rsp\n");
    str_cat(asmg->fun_body, "    popq    %rbp\n");
    str_cat(asmg->fun_body, "    ret\n");

    // combine the function prologue and body into the final text section
    Str* full_fun_code = str_new("");
    str_cat_str(full_fun_code, asmg->fun_prol);
    str_cat_str(full_fun_code, asmg->fun_body);

    str_cat_str(asmg->text, full_fun_code);
}

static void asme_lexpr(AsmG* asmg, const ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_VARREF:
            asme_varaddr(asmg, &expr_node->varaddr_node);
            break;

        case NODE_UNARYOP:
            asme_l_unarop(asmg, &expr_node->unarop_node);
            break;

        default:
            UNREACHABLE();
    }
}

static void asme_expr(AsmG* asmg, const ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_NUM:
            asme_num(asmg, &expr_node->num_node);
            break;

        case NODE_VARREF:
            asme_varref(asmg, &expr_node->varref_node);
            break;

        case NODE_VARADDR:
            asme_varaddr(asmg, &expr_node->varaddr_node);
            break;

        case NODE_BINOP:
            asme_binop(asmg, &expr_node->binop_node);
            break;

        case NODE_UNARYOP:
            asme_unarop(asmg, &expr_node->unarop_node);
            break;

        case NODE_FUNCALL:
            asme_funcall(asmg, &expr_node->funcall_node);
            break;

        case NODE_SYSC:
            asme_syscall(asmg, &expr_node->sysc_node);
            break;

        default:
            UNREACHABLE();
    }
}

static void asme_stmt(AsmG* asmg, const StmtNode* stmt_node)
{
    switch (stmt_node->type) {
        case NODE_VARDEC:
            asme_vardec(asmg, &stmt_node->vardec_node);
            break;

        case NODE_ARRAYDEC:
            asme_arraydec(asmg, &stmt_node->arraydec_node);
            break;

        case NODE_ASS:
            asme_ass(asmg, &stmt_node->ass_node);
            break;

        case NODE_RET:
            asme_ret(asmg, &stmt_node->ret_node);
            break;

        case NODE_EXPR:
            asme_expr(asmg, &stmt_node->expr_node);
            break;

        case NODE_IF:
            asme_if(asmg, &stmt_node->if_node);
            break;

        case NODE_LOOP:
            asme_loop(asmg, &stmt_node->loop_node);
            break;

        case NODE_BREAK:
            asme_break(asmg);
            break;

        default:
            UNREACHABLE();
    }
}

static void init_asm_file(AsmG* asmg)
{
    str_cat(asmg->data, "    .section .data\n");

    str_cat(asmg->bss, "\n    .section .bss\n");

    str_cat(asmg->text_header, "\n    .section .text\n");
}

Str* asme(const List* fundef_node_list)
{
    AsmG* asmg = asmg_new(fundef_node_list);

    // initialize the assembly file with necessary sections
    init_asm_file(asmg);

    // generate assembly code for each function definition node.
    for (int i = 0; i < fundef_node_list->size; i++) {
        const FundefNode* fundef_node = (FundefNode*)fundef_node_list->items[i];
        asme_fundef(asmg, fundef_node);
    }

    // concatenate all sections into the final assembly code
    Str* asm_code = str_new("");
    str_cat_str(asm_code, asmg->data);
    str_cat_str(asm_code, asmg->bss);

    str_cat_str(asm_code, asmg->text_header);
    str_cat_str(asm_code, asmg->global);
    str_cat_str(asm_code, asmg->start);
    str_cat_str(asm_code, asmg->text);
    str_cat(asm_code, "\n    .section .note.GNU-stack,\"\",@progbits\n"); // non-executable stack.

    asmg_free(asmg);
    return asm_code;
}
