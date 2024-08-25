/*
THINGS CHECKED BY THE SEMANTIC ANALYZER:
- assignment to undeclared variables
- references to undeclared (or undefined) variables
- checks if the user is using a different size than <64> for their variables (throws an error if true)
- checks if the user is using a different size than <64> for their function parameters (throws an error if true)
- checks if arrays are declared with a size of 64 bits for their elements
- verifies that the size of the initialized array matches its declared size
- usage of a `break` statement outside of a loop
- usage of an `else` statement outside of an `if` block (ensured by the block structure)
- checks that syscalls have exactly seven arguments
- disallows function calls used as arguments within other function calls
- checks for the redeclaration of an already declared variable
 */

#include <stdlib.h> // malloc(), free()
#include <string.h> // strcmp()

#include "compiler.h"
#include "node.h"
#include "lib.h"

// context for the semantic analyzer containing the symbol table and state flags
typedef struct {
    Dict* symbol_table;
    Bool in_loop;
    Bool in_if;
} AnalyzerContext;

static AnalyzerContext* analyzer_context_new()
{
    AnalyzerContext* context = safe_malloc(sizeof(AnalyzerContext));
    context->symbol_table = dict_new();  // initialize the symbol table
    context->in_loop = FALSE;  // initialize loop state
    context->in_if = FALSE;    // initialize if state
    return context;
}

static void analyzer_context_free(AnalyzerContext* context)
{
    // free the symbol table
    // dict_free(context->symbol_table);
    free(context);
}

static void analyze_stmt(AnalyzerContext* context, StmtNode* stmt_node);
static void analyze_expr(AnalyzerContext* context, ExprNode* expr_node);

static void analyze_block(AnalyzerContext* context, BlockNode* block_node)
{
    for (int i = 0; i < block_node->stmt_node_list->size; i++) {
        StmtNode* stmt_node = (StmtNode*)list_get(block_node->stmt_node_list, i);
        analyze_stmt(context, stmt_node);  // analyze each statement in the block
    }
}

static void check_variable_declared(AnalyzerContext* context, const char* name, int line)
{
    if (dict_get(context->symbol_table, name) == NULL) {
        USER_PANIC(current_filename, line, "variable '%s' referenced before declaration", name);
    }
}

static void analyze_assignment(AnalyzerContext* context, AssNode* ass_node, int line)
{
    if (ass_node->lvalue->type == NODE_VARREF) {
        check_variable_declared(context, ass_node->lvalue->varref_node.name, line);  // check if the variable being assigned is declared
    }
    analyze_expr(context, ass_node->value);  // analyze the right-hand side expression
}

static void analyze_vardec(AnalyzerContext* context, VardecNode* vardec_node, int line)
{
    if (strcmp(vardec_node->size, "64") != 0) {  // ensure the variable size is 64 bits
        USER_PANIC(current_filename, line, "only 64-bit variables are allowed, but '%s' is of size <%s>", vardec_node->name, vardec_node->size);
    }

    if (dict_get(context->symbol_table, vardec_node->name) != NULL) {  // check if the variable has already been declared
        USER_PANIC(current_filename, line, "variable '%s' already declared", vardec_node->name);
    }

    dict_put(context->symbol_table, vardec_node->name, "64");  // add the variable to the symbol table

    if (vardec_node->value) {  // if there is an assignment, analyze the expression
        analyze_expr(context, vardec_node->value);
    }
}

static void analyze_arraydec(AnalyzerContext* context, ArraydecNode* arraydec_node, int line)
{
    if (strcmp(arraydec_node->item_size, "64") != 0) {  // ensure the array element size is 64 bits
        USER_PANIC(current_filename, line, "only 64-bit variables are allowed, but array '%s' contains elements of size <%s>", arraydec_node->name, arraydec_node->item_size);
    }

    if (dict_get(context->symbol_table, arraydec_node->name) != NULL) {  // check if the array has already been declared
        USER_PANIC(current_filename, line, "array '%s' already declared", arraydec_node->name);
    }

    dict_put(context->symbol_table, arraydec_node->name, "64");  // add the array to the symbol table

    if (arraydec_node->expr_node_list) {  // if there is an initialization list, analyze it
        int actual_size = arraydec_node->expr_node_list->size;
        int declared_size = atoi(arraydec_node->size);

        if (declared_size != actual_size) {  // check if the declared size matches the number of elements
            USER_PANIC(current_filename, line, "array '%s' declared with size [%s] but initialized with %d elements", arraydec_node->name, arraydec_node->size, actual_size);
        }

        for (int i = 0; i < arraydec_node->expr_node_list->size; i++) {
            analyze_expr(context, list_get(arraydec_node->expr_node_list, i));  // analyze each initializer expression
        }
    }
}

static void analyze_expr(AnalyzerContext* context, ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_VARREF:
            check_variable_declared(context, expr_node->varref_node.name, expr_node->line); 
            break;

        case NODE_VARADDR:
            check_variable_declared(context, expr_node->varaddr_node.name, expr_node->line);  
            break;

        case NODE_BINOP:
            analyze_expr(context, expr_node->binop_node.left); 
            analyze_expr(context, expr_node->binop_node.right); 
            break;

        case NODE_UNARYOP:
            analyze_expr(context, expr_node->unarop_node.operand);
            break;

        case NODE_FUNCALL:
            for (int i = 0; i < expr_node->funcall_node.expr_node_list->size; i++) {  // analyze each function argument
                ExprNode* arg_node = list_get(expr_node->funcall_node.expr_node_list, i);
                if (arg_node->type == NODE_FUNCALL) {  // disallow nested function calls as arguments
                    USER_PANIC(current_filename, arg_node->line, "function call '%s' used as an argument is not allowed", arg_node->funcall_node.name);
                }
                analyze_expr(context, arg_node);  // analyze the argument
            }
            break;

        case NODE_SYSC:
            if (expr_node->sysc_node.expr_node_list->size != 7) {  // ensure syscalls have exactly seven arguments
                USER_PANIC(current_filename, expr_node->line, "syscall should contain exactly seven arguments");
            }
            for (int i = 0; i < expr_node->sysc_node.expr_node_list->size; i++) {
                analyze_expr(context, list_get(expr_node->sysc_node.expr_node_list, i));  // analyze each syscall argument
            }
            break;

        default:
            break;
    }
}

static void analyze_stmt(AnalyzerContext* context, StmtNode* stmt_node)
{
    switch (stmt_node->type) {
        case NODE_VARDEC:
            analyze_vardec(context, &stmt_node->vardec_node, stmt_node->line); 
            break;

        case NODE_ARRAYDEC:
            analyze_arraydec(context, &stmt_node->arraydec_node, stmt_node->line); 
            break;

        case NODE_ASS:
            analyze_assignment(context, &stmt_node->ass_node, stmt_node->line);  
            break;

        case NODE_RET:
            if (stmt_node->ret_node.expr_node) { 
                analyze_expr(context, stmt_node->ret_node.expr_node);
            }
            break;

        case NODE_IF:
            context->in_if = TRUE;
            analyze_expr(context, stmt_node->if_node.cond_node); 
            analyze_block(context, stmt_node->if_node.true_block); 
            if (stmt_node->if_node.false_block) { 
                analyze_block(context, stmt_node->if_node.false_block);
            }
            context->in_if = FALSE;
            break;

        case NODE_LOOP:
            context->in_loop = TRUE;
            analyze_block(context, stmt_node->loop_node.block);  
            context->in_loop = FALSE;
            break;

        case NODE_BREAK:
            if (!context->in_loop) {  
                USER_PANIC(current_filename, stmt_node->line, "break statement used outside of a loop");
            }
            break;

        case NODE_EXPR:
            analyze_expr(context, &stmt_node->expr_node); 
            break;

        default:
            PANIC("Unhandled statement node type: %d", stmt_node->line, stmt_node->type); 
    }
}

// analyzes a function definition
static void analyze_fundef(AnalyzerContext* context, FundefNode* fundef_node)
{
    context->symbol_table = dict_new();  // reset the symbol table for each function

    for (int i = 0; i < fundef_node->param_node_list->size; i++) {  // analyze function parameters
        ParamNode* param_node = (ParamNode*)list_get(fundef_node->param_node_list, i);
        if (strcmp(param_node->size, "64") != 0) {  // ensure all parameters are 64-bit
            USER_PANIC(current_filename, param_node->line, "only 64-bit parameters are allowed, but parameter '%s' is of size <%s>", param_node->name, param_node->size);
        }
        dict_put(context->symbol_table, param_node->name, param_node->size);  // add parameter to the symbol table
    }

    for (int i = 0; i < fundef_node->block_node->stmt_node_list->size; i++) {
        analyze_stmt(context, (StmtNode*)list_get(fundef_node->block_node->stmt_node_list, i)); 
    }

    dict_free(context->symbol_table);  
}

void analyze_ast(List* fundef_node_list)
{
    AnalyzerContext* context = analyzer_context_new();

    for (int i = 0; i < fundef_node_list->size; i++) {
        FundefNode* fundef_node = (FundefNode*)list_get(fundef_node_list, i);
        analyze_fundef(context, fundef_node); 
    }

    analyzer_context_free(context);  
}
