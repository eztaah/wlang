/*
THINGS CATCH BY THE SEMANTIC ANALYSER
- assignement sur des variables non declarés
- reference à des variables non declarés (ou non definis)
- si l'utilisateur utilise une taille differente de <64> pour ces variables (jette une erreur)
- utilisation d'un break alors que l'on est pas dans un "loop"
- utilisation d'un else alors que l'on est pas dans un "if"
- si des statements sont ecrit en dehors des fonctions
- redeclaration d'une variable deja declarée
 */

#include "lib.h"
#include "node.h"
#include <stdlib.h> // malloc(), free()
#include <string.h> // strcmp()

typedef struct {
    Dict* symbol_table;
    Bool in_loop;
    Bool in_if;
} AnalyzerContext;

static AnalyzerContext* analyzer_context_new()
{
    AnalyzerContext* context = malloc(sizeof(AnalyzerContext));
    context->symbol_table = dict_new();
    context->in_loop = FALSE;
    context->in_if = FALSE;
    return context;
}

static Void analyzer_context_free(AnalyzerContext* context)
{
    // dict_free(context->symbol_table);
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
static Void check_variable_declared(AnalyzerContext* context, const Char* name, I32 line_number)
{
    if (dict_get(context->symbol_table, name) == NULL) {
        USER_PANIC("Line %d: Variable '%s' referenced before declaration", line_number, name);
    }
}

// Analyze assignment
static Void analyze_assignment(AnalyzerContext* context, AssNode* ass_node, I32 line_number)
{
    if (ass_node->lvalue->type == NODE_VARREF) {
        check_variable_declared(context, ass_node->lvalue->varref_node.name, line_number);
    }
    analyze_expr(context, ass_node->value);
}

// Analyze variable declaration
static Void analyze_vardec(AnalyzerContext* context, VardecNode* vardec_node, I32 line_number)
{
    if (strcmp(vardec_node->size, "64") != 0) {
        USER_PANIC("Line %d: Only 64-bit variables are allowed, but '%s' is of size <%s>", line_number, vardec_node->name, vardec_node->size);
    }

    if (dict_get(context->symbol_table, vardec_node->name) != NULL) {
        USER_PANIC("Line %d: Variable '%s' already declared", line_number, vardec_node->name);
    }

    dict_put(context->symbol_table, vardec_node->name, "64");

    if (vardec_node->value) {
        analyze_expr(context, vardec_node->value);
    }
}

// Analyze array declaration
static Void analyze_arraydec(AnalyzerContext* context, ArraydecNode* arraydec_node, I32 line_number)
{
    if (strcmp(arraydec_node->item_size, "64") != 0) {
        USER_PANIC("Line %d: Only 64-bit variables are allowed, but array '%s' contains elements of size <%s>", line_number, arraydec_node->name, arraydec_node->item_size);
    }

    if (dict_get(context->symbol_table, arraydec_node->name) != NULL) {
        USER_PANIC("Line %d: Array '%s' already declared", line_number, arraydec_node->name);
    }

    dict_put(context->symbol_table, arraydec_node->name, "64");

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
            check_variable_declared(context, expr_node->varref_node.name, expr_node->line_number);
            break;

        case NODE_ADDRDEREF:
            analyze_expr(context, expr_node->addrderef_node.expr);
            break;

        case NODE_VARADDR:
            check_variable_declared(context, expr_node->varaddr_node.name, expr_node->line_number);
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
            analyze_vardec(context, &stmt_node->vardec_node, stmt_node->line_number);
            break;

        case NODE_ARRAYDEC:
            analyze_arraydec(context, &stmt_node->arraydec_node, stmt_node->line_number);
            break;

        case NODE_ASS:
            analyze_assignment(context, &stmt_node->ass_node, stmt_node->line_number);
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
                USER_PANIC("Line %d: Break statement used outside of a loop", stmt_node->line_number);
            }
            break;

        case NODE_EXPR:
            analyze_expr(context, &stmt_node->expr_node);
            break;

        default:
            PANIC("Line %d: Unhandled statement node type: %d", stmt_node->line_number, stmt_node->type);
    }
}

// Analyze function definition
static Void analyze_fundef(AnalyzerContext* context, FundefNode* fundef_node)
{
    context->symbol_table = dict_new();

    // Analyze parameters
    for (I32 i = 0; i < fundef_node->param_node_list->size; i++) {
        ParamNode* param_node = (ParamNode*)list_get(fundef_node->param_node_list, i);
        if (strcmp(param_node->size, "64") != 0) {
            USER_PANIC("Line %d: Only 64-bit parameters are allowed, but parameter '%s' is of size <%s>", fundef_node->line_number, param_node->name, param_node->size);
        }
        dict_put(context->symbol_table, param_node->name, param_node->size);
    }

    // Analyze function body
    for (I32 i = 0; i < fundef_node->block_node->stmt_node_list->size; i++) {
        analyze_stmt(context, (StmtNode*)list_get(fundef_node->block_node->stmt_node_list, i));
    }

    dict_free(context->symbol_table);
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
