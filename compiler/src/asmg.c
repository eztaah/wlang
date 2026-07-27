#include <stdio.h>  // sprintf()
#include <stdlib.h> // calloc()
#include <string.h> // strcmp()

#include "compiler.h"

typedef struct {
    List* loop_end_labels;
} LoopLabelStack;

static LoopLabelStack* loop_label_stack_new()
{
    LoopLabelStack* stack = calloc(1, sizeof(LoopLabelStack));
    stack->loop_end_labels = list_new(sizeof(Char*));
    return stack;
}

static Void loop_label_stack_free(LoopLabelStack* stack)
{
    for (I32 i = 0; i < stack->loop_end_labels->size; i++) {
        free(stack->loop_end_labels->items[i]);
    }
    list_free(stack->loop_end_labels);
    free(stack);
}

static Void loop_label_stack_push(LoopLabelStack* stack, const Char* label)
{
    list_push(stack->loop_end_labels, strdup(label));
}

static Char* loop_label_stack_pop(LoopLabelStack* stack)
{
    return list_pop(stack->loop_end_labels);
}

static Char* loop_label_stack_top(LoopLabelStack* stack)
{
    return (Char*)stack->loop_end_labels->items[stack->loop_end_labels->size - 1];
}

typedef struct AsmG {
    const List* node_list;
    Dict* variables; // associate variable names with their stack_offset
    Dict* arrays;    // associate variable names with their stack_offset
    I32 rbp_offset;  // store the current var offset
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
static Void asme_expr(AsmG* asmg, const ExprNode* expr_node);
static Void asme_stmt(AsmG* asmg, const StmtNode* stmt_node);

static AsmG* asmg_new(const List* node_list)
{
    AsmG* asmg = calloc(1, sizeof(AsmG));
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

static Void asmg_free(AsmG* asmg)
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

static Void asme_num(AsmG* asmg, const NumNode* num_node)
{
    str_cat(asmg->fun_body, "    movq    $");
    str_cat(asmg->fun_body, num_node->value);
    str_cat(asmg->fun_body, ", %rax\n");
}

static Void asme_varref(AsmG* asmg, const VarrefNode* varref_node)
{

    // combine function name and variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varref_node->name);

    // check if the variable is in the dict
    I32 var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    I32 array_pos = dict_get(asmg->arrays, str_to_char(full_var_name));
    if (var_pos == -1 && array_pos == -1) {
        PANIC("Reference to %s, but this variable is not defined (should be catch by the semantic anlayser)", str_to_char(full_var_name));
    }

    // handle variable reference
    if (var_pos != -1) {
        Char var_pos_c[256];
        sprintf(var_pos_c, "%d(%%rbp)", var_pos);

        str_cat(asmg->fun_body, "    movq    ");
        str_cat(asmg->fun_body, var_pos_c);
        str_cat(asmg->fun_body, ", %rax\n");
    }

    // handle arrays reference
    else {
        Char array_pos_c[256];
        sprintf(array_pos_c, "%d(%%rbp)", array_pos);

        str_cat(asmg->fun_body, "    leaq    ");
        str_cat(asmg->fun_body, array_pos_c);
        str_cat(asmg->fun_body, ", %rax\n");
    }
}

static Void asme_varaddr(AsmG* asmg, const VaraddrNode* varaddr_node)
{
    // combine function name and variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, varaddr_node->name);

    // check if the variable is in the dict
    I32 var_pos = dict_get(asmg->variables, str_to_char(full_var_name));
    I32 array_pos = dict_get(asmg->arrays, str_to_char(full_var_name));
    if (var_pos == -1 && array_pos == -1) {
        PANIC("Reference to %s, but this variable is not defined (should be catch by the semantic analyser)", str_to_char(full_var_name));
    }

    if (array_pos != -1) {
        USER_PANIC("You want to get the address of an array, but the variable in itself is already an address");
    }

    Char var_pos_c[256];
    sprintf(var_pos_c, "%d(%%rbp)", var_pos);

    str_cat(asmg->fun_body, "    leaq   ");
    str_cat(asmg->fun_body, var_pos_c);
    str_cat(asmg->fun_body, ", %rax\n");
}

static Void asme_addrderef(AsmG* asmg, const AddrderefNode* addrderef_node)
{
    asme_expr(asmg, addrderef_node->expr);                 // the result will be in rax (should be an adress)
    str_cat(asmg->fun_body, "    movq    (%rax), %rbx\n"); // dereference the content of rax and put it into rbx
    str_cat(asmg->fun_body, "    movq    %rbx, %rax\n");   // put the result back into rax
}

static Void asme_l_addrderef(AsmG* asmg, const AddrderefNode* addrderef_node)
{
    asme_expr(asmg, addrderef_node->expr); // the result will be in rax (should be an adress)
}

static void asme_binop(AsmG* asmg, const BinopNode* binop_node)
{
#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # < binop\n");
#endif

    // evaluate the right operand and place it in %rax
    asme_expr(asmg, binop_node->right);
    str_cat(asmg->fun_body, "    pushq   %rax\n"); // push the result onto the stack (%rax is pushed)

    // evaluate the left operand and place it in %rax
    asme_expr(asmg, binop_node->left);
    str_cat(asmg->fun_body, "    pop     %rbx\n"); // pop the right operand into %rbx (pop from stack to %rbx)

    // generate code based on the operation type
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
            str_cat(asmg->fun_body, "    cqo\n");
            str_cat(asmg->fun_body, "    idiv    %rbx\n");
            break;

        case TOKEN_PERCENT:
            str_cat(asmg->fun_body, "    cqo\n");
            str_cat(asmg->fun_body, "    idiv    %rbx\n");
            str_cat(asmg->fun_body, "    movq    %rdx, %rax\n"); // the remainder of the division is in %rdx
            break;

        case TOKEN_LEFTSHIFT:
            str_cat(asmg->fun_body, "    shl     %cl, %rax\n"); // %cl contains the number of bits to shift
            break;

        case TOKEN_RIGHTSHIFT:
            str_cat(asmg->fun_body, "    shr     %cl, %rax\n"); // %cl contains the number of bits to shift
            break;

        case TOKEN_AMPERSAND:
            str_cat(asmg->fun_body, "    and     %rbx, %rax\n");
            break;

        case TOKEN_CARET:
            str_cat(asmg->fun_body, "    xor     %rbx, %rax\n");
            break;

        case TOKEN_PIPE:
            str_cat(asmg->fun_body, "    or      %rbx, %rax\n");
            break;

        case TOKEN_AND:
            // generate code for && (logical AND)
            str_cat(asmg->fun_body, "    cmpq    $0, %rax\n");
            str_cat(asmg->fun_body, "    setne   %al\n");       // if %rax is not zero, %al = 1
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            str_cat(asmg->fun_body, "    cmpq    $0, %rbx\n");
            str_cat(asmg->fun_body, "    setne   %bl\n"); // if %rbx is not zero, %bl = 1
            str_cat(asmg->fun_body, "    movzbq  %bl, %rbx\n");
            str_cat(asmg->fun_body, "    and     %rbx, %rax\n");
            break;

        case TOKEN_OR:
            // generate code for || (logical OR)
            str_cat(asmg->fun_body, "    cmpq    $0, %rax\n");
            str_cat(asmg->fun_body, "    setne   %al\n");       // if %rax is not zero, %al = 1
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            str_cat(asmg->fun_body, "    cmpq    $0, %rbx\n");
            str_cat(asmg->fun_body, "    setne   %bl\n"); // if %rbx is not zero, %bl = 1
            str_cat(asmg->fun_body, "    movzbq  %bl, %rbx\n");
            str_cat(asmg->fun_body, "    or      %rbx, %rax\n");
            break;

        case TOKEN_EQUAL_EQUAL:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    sete    %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            break;

        case TOKEN_NOT_EQUAL:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setne   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            break;

        case TOKEN_LESSTHAN:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setl    %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            break;

        case TOKEN_GREATERTHAN:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setg    %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            break;

        case TOKEN_LESSTHAN_EQ:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setle   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            break;

        case TOKEN_GREATERTHAN_EQ:
            str_cat(asmg->fun_body, "    cmpq    %rbx, %rax\n");
            str_cat(asmg->fun_body, "    setge   %al\n");
            str_cat(asmg->fun_body, "    movzbq  %al, %rax\n"); // zero-extend %al to 64 bits
            break;

        default:
            UNREACHABLE();
    }

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # binop >\n");
#endif
}

static Void asme_unarop(AsmG* asmg, const UnaropNode* unarop_node)
{
    if (unarop_node->op == TOKEN_MINUS) {
        str_cat(asmg->fun_body, "    movq    $");
        str_cat(asmg->fun_body, "-"); // hardcoded but it is ok for now
        str_cat(asmg->fun_body, unarop_node->operand->num_node.value);
        str_cat(asmg->fun_body, ", %rax\n");
    }
    else {
        PANIC("We are only able to convert into asm minus symbol unarop node");
    }
}

static Void asme_funcall(AsmG* asmg, const FuncallNode* funcall_node)
{

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # funcall\n");
#endif

    // handle arguments
    I32 num_args = funcall_node->expr_node_list->size;

    // process the first 6 arguments, mapping them to registers
    const Char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (I32 i = 0; i < num_args && i < 6; i++) {
        const ExprNode* arg_expr = (ExprNode*)funcall_node->expr_node_list->items[i];
        asme_expr(asmg, arg_expr); // the result of the expression should be in %rax
        str_cat(asmg->fun_body, "    movq    %rax, ");
        str_cat(asmg->fun_body, reg_names[i]);
        str_cat(asmg->fun_body, "\n");
    }

    // call the function
    str_cat(asmg->fun_body, "    call    ");
    str_cat(asmg->fun_body, funcall_node->name);
    str_cat(asmg->fun_body, "\n");
}

static Void asme_syscall(AsmG* asmg, const SyscNode* sysc_node)
{

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # syscall\n");
#endif

    // handle arguments
    I32 num_args = sysc_node->expr_node_list->size;

    // process the first 6 arguments, mapping them to registers
    const Char* reg_names[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    for (I32 i = 0; i < num_args && i < 6; i++) {
        const ExprNode* arg_expr = (ExprNode*)sysc_node->expr_node_list->items[i + 1];
        asme_expr(asmg, arg_expr); // the result of the expression should be in %rax
        str_cat(asmg->fun_body, "    movq    %rax, ");
        str_cat(asmg->fun_body, reg_names[i]);
        str_cat(asmg->fun_body, "\n");
    }

    // process rax argument
    const ExprNode* arg_expr = (ExprNode*)sysc_node->expr_node_list->items[0];
    asme_expr(asmg, arg_expr); // the result of the expression should be in %rax

    // Clean stack if the syscall is a syscall exit
    // TOFIX: Will not work as expected if the error_code is not a number node
    if (strcmp(((ExprNode*)sysc_node->expr_node_list->items[0])->num_node.value, "60") == 0) {
#ifdef ASM_COMMENTS
        str_cat(asmg->fun_body, "\n    # clean stack\n");
#endif
        str_cat(asmg->fun_body, "    movq    %rbp, %rsp\n");
        str_cat(asmg->fun_body, "    pop     %rbp\n");
    }

    // call the function
    str_cat(asmg->fun_body, "    syscall\n");
}

static Void asme_lexpr(AsmG* asmg, const ExprNode* expr_node);

static Void asme_ass(AsmG* asmg, const AssNode* ass_node)
{

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "\n    # variables assignement\n");
#endif

    // get the lvalue and put it into rbx       (rbx will contain a location)
    asme_lexpr(asmg, ass_node->lvalue);
    str_cat(asmg->fun_body, "    movq    %rax, %r15\n");

    // get the value of the variable and put it into rax
    asme_expr(asmg, ass_node->value); // the value of the variable will be in rax

    // instruction
    str_cat(asmg->fun_body, "    movq    %rax, (%r15)\n");
}

static Void asme_if(AsmG* asmg, const IfNode* if_node)
{
    static I32 if_label_counter = 0; // Unique label counter for if statements
    I32 current_label = if_label_counter++;

    // evaluate the condition
    asme_expr(asmg, if_node->cond_node);
    str_cat(asmg->fun_body, "    cmpq    $0, %rax\n"); // Compare the result with 0

    // jump to the false block if condition is false
    Char label_false[64];
    sprintf(label_false, "    je      .Lelse%d\n", current_label);
    str_cat(asmg->fun_body, label_false);

    // generate code for each stmt of the true block
    for (I32 i = 0; i < if_node->true_block->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)if_node->true_block->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }

    // jump to the end after executing true block
    Char label_end[64];
    sprintf(label_end, "    jmp     .Lend%d\n", current_label);
    str_cat(asmg->fun_body, label_end);

    // false block
    str_cat(asmg->fun_body, ".Lelse");
    sprintf(label_false, "%d:\n", current_label);
    str_cat(asmg->fun_body, label_false);
    if (if_node->false_block) {
        // generate code for each stmt of the true block
        for (I32 i = 0; i < if_node->false_block->stmt_node_list->size; i++) {
            const StmtNode* stmt_node = (StmtNode*)if_node->false_block->stmt_node_list->items[i];
            asme_stmt(asmg, stmt_node);
        }
    }

    // end label
    str_cat(asmg->fun_body, ".Lend");
    sprintf(label_end, "%d:\n", current_label);
    str_cat(asmg->fun_body, label_end);
}

static Void asme_loop(AsmG* asmg, const LoopNode* loop_node)
{
    static I32 loop_label_counter = 0; // Unique label counter for loops
    I32 current_label = loop_label_counter++;

    // generate loop labels
    Char label_start[64];
    Char label_end[64];
    sprintf(label_start, ".Lloop%d", current_label);
    sprintf(label_end, ".Lloopend%d", current_label);

    // push the end label onto the loop stack
    loop_label_stack_push(asmg->loop_stack, label_end);

    // loop start label
    str_cat(asmg->fun_body, label_start);
    str_cat(asmg->fun_body, ":\n");

    // generate code for each stmt in the loop block
    for (I32 i = 0; i < loop_node->block->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)loop_node->block->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }

    // jump back to start for the next iteration
    str_cat(asmg->fun_body, "    jmp     ");
    str_cat(asmg->fun_body, label_start);
    str_cat(asmg->fun_body, "\n");

    // loop end label (for break statements)
    str_cat(asmg->fun_body, label_end);
    str_cat(asmg->fun_body, ":\n");

    // pop the end label from the loop stack
    loop_label_stack_pop(asmg->loop_stack);
}

static Void asme_break(AsmG* asmg)
{
    // use the top label from the loop stack
    Char* label_end = loop_label_stack_top(asmg->loop_stack);
    str_cat(asmg->fun_body, "    jmp     ");
    str_cat(asmg->fun_body, label_end);
    str_cat(asmg->fun_body, "\n");
}

static Void asme_vardec(AsmG* asmg, const VardecNode* vardec_node)
{

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "\n    # variables declaration\n");
#endif

    // get full variable name
    Str* full_var_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_var_name, "_");
    str_cat(full_var_name, vardec_node->name);

    // is the variable is not in the dict, add it in the dict

    dict_put(asmg->variables, str_to_char(full_var_name), asmg->rbp_offset);

    print(MSG_INFO, "add in the dict: \"%s\" at location %d\n", str_to_char(full_var_name), asmg->rbp_offset);

    // process expression only if it exists
    if (vardec_node->value != NULL) {
        asme_expr(asmg, vardec_node->value); // the value of the variable will be in rax

        Char var_location[256];
        sprintf(var_location, "%d(%%rbp)", asmg->rbp_offset);

        str_cat(asmg->fun_body, "    movq    %rax, ");
        str_cat(asmg->fun_body, var_location);
        str_cat(asmg->fun_body, "\n");
    }

    asmg->rbp_offset -= 8;
}

static Void asme_arraydec(AsmG* asmg, const ArraydecNode* arraydec_node)
{

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "\n    # array declaration\n");
#endif

    // get full array name
    Str* full_array_name = str_new(str_to_char(asmg->var_prefix));
    str_cat(full_array_name, "_");
    str_cat(full_array_name, arraydec_node->name);

    // is the array is not in the dict, add it in the dict

    dict_put(asmg->arrays, str_to_char(full_array_name), asmg->rbp_offset);
    // reserve space on the stack for the whole array

    print(MSG_INFO, "add array 1st elmt in the dict: \"%s\" at location %d\n", str_to_char(full_array_name), asmg->rbp_offset);

    // convert array size to int
    I32 array_size = atoi(arraydec_node->size);
    // process expression only if it exists
    if (arraydec_node->expr_node_list != NULL) {
        I32 current_offset = asmg->rbp_offset;

        for (int i = 0; i < array_size; i++) {
            // get the expression
            const ExprNode* arg_expr = (ExprNode*)arraydec_node->expr_node_list->items[i];
            asme_expr(asmg, arg_expr); // the result of the expression should be in %rax

            Char var_location[256];
            current_offset -= 8;

            sprintf(var_location, "%d(%%rbp)", current_offset);

            str_cat(asmg->fun_body, "    movq    %rax, ");
            str_cat(asmg->fun_body, var_location);
            str_cat(asmg->fun_body, "\n");
        }
    }

    asmg->rbp_offset -= 8 * array_size; // we need to do that here in case you define an array without initializing it
}

static Void asme_ret(AsmG* asmg, const RetNode* ret_node)
{

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "\n    # return statement\n");
#endif

    // handle expression
    asme_expr(asmg, ret_node->expr_node); // this will put the value into rax

#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # function epilogue\n");
#endif

    str_cat(asmg->fun_body, "    movq    %rbp, %rsp\n");
    str_cat(asmg->fun_body, "    pop     %rbp\n");
    str_cat(asmg->fun_body, "    ret\n"); // we can use rax after to use the return value
}

static Void asme_fundef(AsmG* asmg, const FundefNode* fundef_node)
{
    if (strcmp(fundef_node->name, "w__main") == 0) {
        // start code
        str_cat(asmg->global, "    .global _start\n");
        str_cat(asmg->start, "\n_start:\n");
        str_cat(asmg->start, "    call w__main\n");
        str_cat(asmg->start, "    movq %rax, %rdi\n");
        str_cat(asmg->start, "    movq $60, %rax\n");
        str_cat(asmg->start, "    syscall\n");
    }

    // check if function is global
    if (strcmp(fundef_node->scope, "global") == 0) {
        str_cat(asmg->global, "    .global ");
        str_cat(asmg->global, fundef_node->name);
        str_cat(asmg->global, "\n");
    }

    // clear fun_body and fun_prol
    str_free(asmg->fun_prol);
    asmg->fun_prol = str_new("");

    str_free(asmg->fun_body);
    asmg->fun_body = str_new("");

    // function label
    str_cat(asmg->fun_prol, "\n");
    str_cat(asmg->fun_prol, fundef_node->name);
    str_cat(asmg->fun_prol, ":\n");

    // function prologue
#ifdef ASM_COMMENTS
    str_cat(asmg->fun_prol, "    # function prologue\n");
#endif
    str_cat(asmg->fun_prol, "    pushq   %rbp\n");
    str_cat(asmg->fun_prol, "    movq    %rsp, %rbp\n");

    // pushing arguments onto the stack frame
#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # storing arguments into stackframe\n");
#endif

    asmg->rbp_offset = -8; // because when we declare the first variable, it need to have space

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
                str_cat(asmg->fun_body, "    movq    %rdi, ");
                break;
            case 1:
                str_cat(asmg->fun_body, "    movq    %rsi, ");
                break;
            case 2:
                str_cat(asmg->fun_body, "    movq    %rdx, ");
                break;
            case 3:
                str_cat(asmg->fun_body, "    movq    %rcx, ");
                break;
            case 4:
                str_cat(asmg->fun_body, "    movq    %r8, ");
                break;
            case 5:
                str_cat(asmg->fun_body, "    movq    %r9, ");
                break;
            default:
                PANIC("Too many arguments");
        }
        Char arg_pos[20];
        sprintf(arg_pos, "%d(%%rbp)", asmg->rbp_offset);
        str_cat(asmg->fun_body, arg_pos);
        str_cat(asmg->fun_body, "\n");

        // decrease the offset for the next argument
        asmg->rbp_offset -= 8;
    }

    // put the function name in var_prefix
    str_free(asmg->var_prefix);
    asmg->var_prefix = str_new(fundef_node->name);

    // generate code for each stmt of the block_node
    for (I32 i = 0; i < fundef_node->block_node->stmt_node_list->size; i++) {
        const StmtNode* stmt_node = (StmtNode*)fundef_node->block_node->stmt_node_list->items[i];
        asme_stmt(asmg, stmt_node);
    }

    // add fun prologue for how many space we want
    I32 required_space = abs(asmg->rbp_offset);
    if (required_space % 2 == 0) {
        required_space += 8;
    }
    Char required_space_str[255];
    sprintf(required_space_str, "%d", required_space);
    str_cat(asmg->fun_prol, "    subq    $");
    str_cat(asmg->fun_prol, required_space_str);
    str_cat(asmg->fun_prol, ", %rsp\n");

    // handle case where the function return nothing
#ifdef ASM_COMMENTS
    str_cat(asmg->fun_body, "    # function epilogue\n");
#endif
    str_cat(asmg->fun_body, "    movq    %rbp, %rsp\n");
    str_cat(asmg->fun_body, "    pop     %rbp\n");
    str_cat(asmg->fun_body, "    ret\n");

    // combine all code
    Str* full_fun_code = str_new("");
    str_cat_str(full_fun_code, asmg->fun_prol);
    str_cat_str(full_fun_code, asmg->fun_body);

    str_cat_str(asmg->text, full_fun_code);
}

static Void asme_lexpr(AsmG* asmg, const ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_VARREF:
            asme_varaddr(asmg, &expr_node->varaddr_node);
            break;

        case NODE_ADDRDEREF:
            asme_l_addrderef(asmg, &expr_node->addrderef_node);
            break;

        default:
            UNREACHABLE();
    }
}

static Void asme_expr(AsmG* asmg, const ExprNode* expr_node)
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

        case NODE_ADDRDEREF:
            asme_addrderef(asmg, &expr_node->addrderef_node);
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

static Void asme_stmt(AsmG* asmg, const StmtNode* stmt_node)
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
    // Setup initial instructions
#ifdef ASM_COMMENTS
    str_cat(asmg->data, "    # --- SECTION DATA ---\n");
#endif
    str_cat(asmg->data, "    .section .data\n");

#ifdef ASM_COMMENTS
    str_cat(asmg->bss, "\n\n    # --- SECTION BSS ---\n");
#endif
    str_cat(asmg->bss, "    .section .bss\n");

#ifdef ASM_COMMENTS
    str_cat(asmg->text_header, "\n\n    # --- SECTION TEXT ---\n");
#endif
    str_cat(asmg->text_header, "    .section .text\n");
}

Str* asme(const List* fundef_node_list)
{
    print(MSG_STEP, "generating assembly...\n");
    AsmG* asmg = asmg_new(fundef_node_list);

    // initialize asm stmtuctions
    init_asm_file(asmg);

    // Generate assembly code for each node
    for (I32 i = 0; i < fundef_node_list->size; i++) {
        const FundefNode* fundef_node = (FundefNode*)fundef_node_list->items[i];
        asme_fundef(asmg, fundef_node);
    }

    // concatenate all code into one string
    Str* asm_code = str_new("");
    str_cat_str(asm_code, asmg->data);
    str_cat_str(asm_code, asmg->bss);

    str_cat_str(asm_code, asmg->text_header);
    str_cat_str(asm_code, asmg->global);
    str_cat_str(asm_code, asmg->start);
    str_cat_str(asm_code, asmg->text);
    str_cat(asm_code, "\n");

    // clean and return
    asmg_free(asmg);
    return asm_code;
}
