#include "node.h"
#include "lib.h"
#include <stdlib.h> // malloc(), free()
#include <string.h> // strcmp()

typedef struct {
    DictStr* symbol_table;
    Bool in_loop;
    Bool in_if;
} AnalyzerContext;

static AnalyzerContext* analyzer_context_new()
{
    AnalyzerContext* context = malloc(sizeof(AnalyzerContext));
    context->symbol_table = dictstr_new();
    context->in_loop = FALSE;
    context->in_if = FALSE;
    return context;
}

static Void analyzer_context_free(AnalyzerContext* context)
{
    // dictstr_free(context->symbol_table);
    free(context);
}

static Void analyze_stmt(AnalyzerContext* context, StmtNode* stmt_node);
static Void analyze_expr(AnalyzerContext* context, ExprNode* expr_node);

static Void analyze_block(AnalyzerContext* context, BlockNode* block_node)
{
    for (I32 i = 0; i < block_node->stmt_node_list->size; i++) {
        StmtNode* stmt_node = (StmtNode*)list_get(block_node->stmt_node_list, i);
        analyze_stmt(context, stmt_node);
    }
}

// Check if the variable is declared
static Void check_variable_declared(AnalyzerContext* context, const Char* name)
{
    if (dictstr_get(context->symbol_table, name) == NULL) {
        USER_PANIC("Variable '%s' referenced before declaration", name);
    }
}

// Analyze assignment
static Void analyze_assignment(AnalyzerContext* context, AssNode* ass_node)
{
    if (ass_node->lvalue->type == NODE_VARREF) {
        check_variable_declared(context, ass_node->lvalue->varref_node.name);
    }
    analyze_expr(context, ass_node->value);
}

// Analyze variable declaration
static Void analyze_vardec(AnalyzerContext* context, VardecNode* vardec_node)
{
    if (strcmp(vardec_node->size, "64") != 0) {
        USER_PANIC("Only 64-bit variables are allowed, but '%s' is of size <%s>", vardec_node->name, vardec_node->size);
    }

    if (dictstr_get(context->symbol_table, vardec_node->name) != NULL) {
        USER_PANIC("Variable '%s' already declared", vardec_node->name);
    }

    dictstr_put(context->symbol_table, vardec_node->name, "64");

    if (vardec_node->value) {
        analyze_expr(context, vardec_node->value);
    }
}

// Analyze array declaration
static Void analyze_arraydec(AnalyzerContext* context, ArraydecNode* arraydec_node)
{
    if (strcmp(arraydec_node->item_size, "64") != 0) {
        USER_PANIC("Only 64-bit variables are allowed, but array '%s' contains elements of size <%s>", arraydec_node->name, arraydec_node->item_size);
    }

    if (dictstr_get(context->symbol_table, arraydec_node->name) != NULL) {
        USER_PANIC("Array '%s' already declared", arraydec_node->name);
    }

    dictstr_put(context->symbol_table, arraydec_node->name, "64");

    if (arraydec_node->expr_node_list) {
        for (I32 i = 0; i < arraydec_node->expr_node_list->size; i++) {
            analyze_expr(context, list_get(arraydec_node->expr_node_list, i));
        }
    }
}

// Analyze expression
static Void analyze_expr(AnalyzerContext* context, ExprNode* expr_node)
{
    switch (expr_node->type) {
        case NODE_VARREF:
            check_variable_declared(context, expr_node->varref_node.name);
            break;

        case NODE_ADDRDEREF:
            analyze_expr(context, expr_node->addrderef_node.expr);
            break;

        case NODE_VARADDR:
            check_variable_declared(context, expr_node->varaddr_node.name);
            break;

        case NODE_BINOP:
            analyze_expr(context, expr_node->binop_node.left);
            analyze_expr(context, expr_node->binop_node.right);
            break;

        case NODE_UNARYOP:
            analyze_expr(context, expr_node->unarop_node.operand);
            break;

        case NODE_FUNCALL:
            for (I32 i = 0; i < expr_node->funcall_node.expr_node_list->size; i++) {
                analyze_expr(context, list_get(expr_node->funcall_node.expr_node_list, i));
            }
            break;

        case NODE_SYSC:
            for (I32 i = 0; i < expr_node->sysc_node.expr_node_list->size; i++) {
                analyze_expr(context, list_get(expr_node->sysc_node.expr_node_list, i));
            }
            break;

        default:
            break;
    }
}

// Analyze statement
static Void analyze_stmt(AnalyzerContext* context, StmtNode* stmt_node)
{
    switch (stmt_node->type) {
        case NODE_VARDEC:
            analyze_vardec(context, &stmt_node->vardec_node);
            break;

        case NODE_ARRAYDEC:
            analyze_arraydec(context, &stmt_node->arraydec_node);
            break;

        case NODE_ASS:
            analyze_assignment(context, &stmt_node->ass_node);
            break;

        case NODE_RET:
            analyze_expr(context, stmt_node->ret_node.expr_node);
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
                USER_PANIC("Break statement used outside of a loop");
            }
            break;

        case NODE_EXPR:
            analyze_expr(context, &stmt_node->expr_node);
            break;

        default:
            PANIC("Unhandled statement node type: %d", stmt_node->type);
    }
}


// Analyze function definition
static Void analyze_fundef(AnalyzerContext* context, FundefNode* fundef_node)
{
    context->symbol_table = dictstr_new();

    // Analyze parameters
    for (I32 i = 0; i < fundef_node->param_node_list->size; i++) {
        ParamNode* param_node = (ParamNode*)list_get(fundef_node->param_node_list, i);
        if (strcmp(param_node->size, "64") != 0) {
            USER_PANIC("Only 64-bit parameters are allowed, but parameter '%s' is of size <%s>", param_node->name, param_node->size);
        }
        dictstr_put(context->symbol_table, param_node->name, param_node->size);
    }

    // Analyze function body
    for (I32 i = 0; i < fundef_node->block_node->stmt_node_list->size; i++) {
        analyze_stmt(context, (StmtNode*)list_get(fundef_node->block_node->stmt_node_list, i));
    }

    dictstr_free(context->symbol_table);
}

// Perform semantic analysis on the AST
Void analyze_ast(List* fundef_node_list)
{
    AnalyzerContext* context = analyzer_context_new();

    for (I32 i = 0; i < fundef_node_list->size; i++) {
        FundefNode* fundef_node = (FundefNode*)list_get(fundef_node_list, i);
        analyze_fundef(context, fundef_node);
    }

    analyzer_context_free(context);
}
