#include "lib.h"
#include "node.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Void print_indent(Str* output, I32 pos_x)
{
    for (I32 i = 0; i < pos_x; ++i) {
        str_cat(output, " ");
    }
}

static Void print_number_node(NumberNode* number_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "NumberNode\n");
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ value: %s\n", number_node->value);
    str_cat(output, buffer);
}

static Void print_var_ref_node(VarRefNode* var_ref_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "VarRefNode\n");
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ name: %s\n", var_ref_node->name);
    str_cat(output, buffer);
}

static Void print_expr_node(ExprNode* node, Str* output, I32 pos_x);

static Void print_binop_node(BinopNode* binop_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    snprintf(buffer, sizeof(buffer), "BinopNode\n");
    str_cat(output, buffer);
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ op: %s\n", tokentype_to_string(binop_node->op));
    str_cat(output, buffer);
    print_indent(output, pos_x);
    str_cat(output, "├─ left:  ");
    print_expr_node(binop_node->left, output, pos_x + 10);
    print_indent(output, pos_x);
    str_cat(output, "└─ right: ");
    print_expr_node(binop_node->right, output, pos_x + 10);
}

static Void print_unaryop_node(UnaryOpNode* unaryop_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    snprintf(buffer, sizeof(buffer), "UnaryopNode\n");
    str_cat(output, buffer);
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ op: %s\n", tokentype_to_string(unaryop_node->op));
    str_cat(output, buffer);
    print_indent(output, pos_x);
    str_cat(output, "└─ operand: ");
    print_expr_node(unaryop_node->operand, output, pos_x + 12);
}

static Void print_expr_node_list(List* expr_nodelist, Str* output, I32 pos_x)
{
    str_cat(output, "List<ExprNode>\n");
    for (int i = 0; i < expr_nodelist->size - 1; i++) {
        print_indent(output, pos_x);
        str_cat(output, "├─ ");
        ExprNode* expr_node = (ExprNode*)list_get(expr_nodelist, i);
        print_expr_node(expr_node, output, pos_x + 3);
    }

    print_indent(output, pos_x);
    str_cat(output, "└─ ");
    ExprNode* expr_node = (ExprNode*)list_get(expr_nodelist, expr_nodelist->size - 1);
    print_expr_node(expr_node, output, pos_x + 3);
}

static Void print_funcall_node(FunCallNode* funcall_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "FunCallNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", funcall_node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "├─ args: ");
    print_expr_node_list(funcall_node->expr_node_list, output, pos_x + 9);
}

static Void print_expr_node(ExprNode* node, Str* output, I32 pos_x)
{

    switch (node->type) {
        case NODE_NUMBER:
            print_number_node(&node->number_node, output, pos_x);
            break;

        case NODE_VAR_REF:
            print_var_ref_node(&node->var_ref_node, output, pos_x);
            break;

        case NODE_BINOP:
            print_binop_node(&node->bin_op_node, output, pos_x);
            break;

        case NODE_UNARYOP:
            print_unaryop_node(&node->unary_op_node, output, pos_x);
            break;

        case NODE_FUN_CALL:
            print_funcall_node(&node->fun_call_node, output, pos_x);
            break;

        default:
            UNREACHABLE();
            break;
    }
}

static Void print_var_def_node(VarDefNode* node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "VarDefNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ mut: \"%s\"\n", node->mut);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ type: \"%s\"\n", node->type);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->value, output, pos_x + 10);
}

static Void print_var_modif_node(VarModifNode* node, Str* output, I32 pos_x)
{
    Char buffer[256];

    print_indent(output, pos_x);
    str_cat(output, "VarModifNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->value, output, pos_x + 10);
}

static Void print_return_node(ReturnNode* node, Str* output, I32 pos_x)
{
    str_cat(output, "ReturnNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->expr_node, output, pos_x + 10);
}

static Void print_instr_node(InstrNode* instr_node, Str* output, I32 pos_x)
{
    switch (instr_node->type) {
        case NODE_VAR_DEF:
            print_var_def_node(&instr_node->var_def, output, pos_x);
            break;

        case NODE_VAR_MODIF:
            print_var_modif_node(&instr_node->var_modif, output, pos_x);
            break;

        case NODE_RETURN:
            print_return_node(&instr_node->return_node, output, pos_x);
            break;

        case NODE_EXPR:
            print_expr_node(&instr_node->expr_node, output, pos_x);
            break;

        default:
            UNREACHABLE();
            break;
    }
}

static Void print_param_node(ParamNode* node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "ParamNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ mut: \"%s\"\n", node->mut);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ type: \"%s\"\n", node->type);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ name: \"%s\"\n", node->name);
    str_cat(output, buffer);
}

static Void print_param_node_list(List* param_node_list, Str* output, I32 pos_x)
{
    str_cat(output, "List<ParamNode>\n");
    for (int i = 0; i < param_node_list->size - 1; i++) {
        print_indent(output, pos_x);
        str_cat(output, "├─ ");
        ParamNode* param_node = (ParamNode*)list_get(param_node_list, i);
        print_param_node(param_node, output, pos_x + 3);
    }

    print_indent(output, pos_x);
    str_cat(output, "└─ ");
    ParamNode* param_node = (ParamNode*)list_get(param_node_list, param_node_list->size - 1);
    print_param_node(param_node, output, pos_x + 3);
}

static Void print_stmt_node(StmtNode* stmt_node, Str* output, I32 pos_x);

static Void print_stmt_node_list(List* stmt_node_list, Str* output, I32 pos_x)
{
    str_cat(output, "List<StmtNode>\n");
    if (stmt_node_list->size > 0) {
        for (int i = 0; i < stmt_node_list->size - 1; i++) {
            print_indent(output, pos_x);
            str_cat(output, "├─ ");
            StmtNode* stmt_node = (StmtNode*)list_get(stmt_node_list, i);
            print_stmt_node(stmt_node, output, pos_x + 3);
        }

        print_indent(output, pos_x);
        str_cat(output, "└─ ");
        StmtNode* stmt_node = (StmtNode*)list_get(stmt_node_list, stmt_node_list->size - 1);
        print_stmt_node(stmt_node, output, pos_x + 3);
    }
    else {
        print_indent(output, pos_x);
        str_cat(output, "└─ NULL\n");
    }
}

static Void print_codeblock_node(CodeblockNode* codeblock_node, Str* output, I32 pos_x)
{
    str_cat(output, "CodeblockNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ content: ");
    print_stmt_node_list(codeblock_node->stmt_node_list, output, pos_x + 12);
}

static Void print_fundef_node(FunDefNode* fundef_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "FunDefNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", fundef_node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ return_type: \"%s\"\n", fundef_node->return_type);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "├─ parameters: ");
    print_param_node_list(fundef_node->param_node_list, output, pos_x + 15);

    print_indent(output, pos_x);
    str_cat(output, "├─ code_block: ");
    print_codeblock_node(fundef_node->code_block, output, pos_x + 15);
}

static Void print_stmt_node(StmtNode* stmt_node, Str* output, I32 pos_x)
{
    switch (stmt_node->type) {
        case NODE_INSTR:
            print_instr_node(&stmt_node->instr_node, output, pos_x);
            break;

        case NODE_FUN_DEF:
            print_fundef_node(&stmt_node->fun_def_node, output, pos_x);
            break;

        default:
            UNREACHABLE();
            break;
    }
}

Str* print_nodelist(const List* node_list)
{
    Str* output = str_new("[\n");

    for (I32 i = 0; i < node_list->size; ++i) {
        StmtNode* stmt_node = (StmtNode*)node_list->items[i];
        print_indent(output, 4);
        print_stmt_node(stmt_node, output, 4);

        // Saute une ligne après chaque statement node
        if (i < node_list->size - 1) {
            str_cat(output, "    ,\n");
        }
    }

    str_cat(output, "\n]");

    return output;
}
