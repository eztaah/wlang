#include <stdio.h> // snprintf()

#include "lib.h"
#include "node.h"

static void print_indent(Str* output, int pos_x)
{
    for (int i = 0; i < pos_x; ++i) {
        str_cat(output, " ");
    }
}

static void print_node_header(const char* node_type, const char* label, const char* value, Str* output, int pos_x)
{
    if (!node_type || !label || !output) {
        return;
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s\n", node_type);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ %s: %s\n", label, value ? value : "NULL");
    str_cat(output, buffer);
}

static void print_node(const char* node_type, const char* value, Str* output, int pos_x)
{
    if (!node_type || !output) {
        return;
    }
    print_node_header(node_type, "value", value, output, pos_x);
}

// Print specific node types with additional information
static void print_expr_node(const ExprNode* node, Str* output, int pos_x);

static void print_num_node(const NumNode* num_node, Str* output, int pos_x)
{
    if (!num_node) {
        print_node("NumNode", "NULL", output, pos_x);
        return;
    }
    print_node("NumNode", num_node->value, output, pos_x);
}

static void print_varref_node(const VarrefNode* varref_node, Str* output, int pos_x)
{
    if (!varref_node) {
        print_node("VarrefNode", "NULL", output, pos_x);
        return;
    }
    print_node("VarrefNode", varref_node->name, output, pos_x);
}

static void print_varaddr_node(const VaraddrNode* varaddr_node, Str* output, int pos_x)
{
    if (!varaddr_node) {
        print_node("VaraddrNode", "NULL", output, pos_x);
        return;
    }
    print_node("VaraddrNode", varaddr_node->name, output, pos_x);
}

static void print_binop_node(const BinopNode* binop_node, Str* output, int pos_x)
{
    if (!binop_node) {
        print_node("BinopNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "BinopNode\n");

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

static void print_unarop_node(const UnaropNode* unarop_node, Str* output, int pos_x)
{
    if (!unarop_node) {
        print_node("UnaryopNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "UnaryopNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ op: %s\n", tokentype_to_string(unarop_node->op));
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ operand: ");
    print_expr_node(unarop_node->operand, output, pos_x + 12);
}

static void print_expr_node_list(const List* expr_nodelist, Str* output, int pos_x)
{
    str_cat(output, "List<ExprNode>\n");

    if (!expr_nodelist || expr_nodelist->size == 0) {
        print_indent(output, pos_x);
        str_cat(output, "└─ EMPTY\n");
        return;
    }

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

static void print_funcall_node(const FuncallNode* funcall_node, Str* output, int pos_x)
{
    if (!funcall_node) {
        print_node("FuncallNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "FuncallNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", funcall_node->name ? funcall_node->name : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ args: ");
    print_expr_node_list(funcall_node->expr_node_list, output, pos_x + 9);
}

static void print_sysc_node(const SyscNode* sysc_node, Str* output, int pos_x)
{
    if (!sysc_node) {
        print_node("SyscNode", "NULL", output, pos_x);
        return;
    }

    str_cat(output, "SyscNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ args: ");
    print_expr_node_list(sysc_node->expr_node_list, output, pos_x + 9);
}

static void print_expr_node(const ExprNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("ExprNode", "NULL", output, pos_x);
        return;
    }

    switch (node->type) {
        case NODE_NUM:
            print_num_node(&node->num_node, output, pos_x);
            break;

        case NODE_VARREF:
            print_varref_node(&node->varref_node, output, pos_x);
            break;

        case NODE_VARADDR:
            print_varaddr_node(&node->varaddr_node, output, pos_x);
            break;

        case NODE_BINOP:
            print_binop_node(&node->binop_node, output, pos_x);
            break;

        case NODE_UNARYOP:
            print_unarop_node(&node->unarop_node, output, pos_x);
            break;

        case NODE_FUNCALL:
            print_funcall_node(&node->funcall_node, output, pos_x);
            break;

        case NODE_SYSC:
            print_sysc_node(&node->sysc_node, output, pos_x);
            break;

        default:
            print_node("Unknown ExprNode Type", "NULL", output, pos_x);
            break;
    }
}

static void print_arraydec_node(const ArraydecNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("ArraydecNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "ArraydecNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ item_size: %s\n", node->item_size ? node->item_size : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: %s\n", node->name ? node->name : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ size: %s\n", node->size ? node->size : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ expr_node_list: ");
    print_expr_node_list(node->expr_node_list, output, pos_x + 17);
}

static void print_vardec_node(const VardecNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("VardecNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "VardecNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ size: %s\n", node->size ? node->size : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: %s\n", node->name ? node->name : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->value, output, pos_x + 10);
}

static void print_stmt_node_list(const List* stmt_node_list, Str* output, int pos_x);

static void print_block_node(const BlockNode* block_node, Str* output, int pos_x)
{
    if (!block_node) {
        print_node("BlockNode", "NULL", output, pos_x);
        return;
    }

    str_cat(output, "BlockNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ content: ");
    print_stmt_node_list(block_node->stmt_node_list, output, pos_x + 12);
}

static void print_if_node(const IfNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("IfNode", "NULL", output, pos_x);
        return;
    }

    str_cat(output, "IfNode\n");

    print_indent(output, pos_x);
    str_cat(output, "├─ condition: ");
    print_expr_node(node->cond_node, output, pos_x + 14);

    print_indent(output, pos_x);
    str_cat(output, "├─ true_block: ");
    print_block_node(node->true_block, output, pos_x + 15);

    print_indent(output, pos_x);
    str_cat(output, "└─ false_block: ");
    print_block_node(node->false_block, output, pos_x + 16);
}

static void print_loop_node(const LoopNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("LoopNode", "NULL", output, pos_x);
        return;
    }

    str_cat(output, "LoopNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ Block:");
    print_block_node(node->block, output, pos_x + 10);
}

static void print_break_node(Str* output, int pos_x)
{
    str_cat(output, "BreakNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ NULL\n");
}

static void print_ass_node(const AssNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("AssNode", "NULL", output, pos_x);
        return;
    }

    str_cat(output, "AssNode\n");

    print_indent(output, pos_x);
    str_cat(output, "├─ lvalue: ");
    print_expr_node(node->lvalue, output, pos_x + 11);

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->value, output, pos_x + 10);
}

static void print_ret_node(const RetNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("RetNode", "NULL", output, pos_x);
        return;
    }

    str_cat(output, "RetNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->expr_node, output, pos_x + 10);
}

static void print_stmt_node(const StmtNode* stmt_node, Str* output, int pos_x)
{
    if (!stmt_node) {
        print_node("StmtNode", "NULL", output, pos_x);
        return;
    }

    switch (stmt_node->type) {
        case NODE_VARDEC:
            print_vardec_node(&stmt_node->vardec_node, output, pos_x);
            break;

        case NODE_ARRAYDEC:
            print_arraydec_node(&stmt_node->arraydec_node, output, pos_x);
            break;

        case NODE_ASS:
            print_ass_node(&stmt_node->ass_node, output, pos_x);
            break;

        case NODE_RET:
            print_ret_node(&stmt_node->ret_node, output, pos_x);
            break;

        case NODE_EXPR:
            print_expr_node(&stmt_node->expr_node, output, pos_x);
            break;

        case NODE_IF:
            print_if_node(&stmt_node->if_node, output, pos_x);
            break;

        case NODE_LOOP:
            print_loop_node(&stmt_node->loop_node, output, pos_x);
            break;

        case NODE_BREAK:
            print_break_node(output, pos_x);
            break;

        default:
            print_node("Unknown StmtNode Type", "NULL", output, pos_x);
            break;
    }
}

static void print_param_node(const ParamNode* node, Str* output, int pos_x)
{
    if (!node) {
        print_node("ParamNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "ParamNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", node->name ? node->name : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ size: \"%s\"\n", node->size ? node->size : "NULL");
    str_cat(output, buffer);
}

static void print_param_node_list(const List* param_node_list, Str* output, int pos_x)
{
    str_cat(output, "List<ParamNode>\n");

    if (!param_node_list || param_node_list->size == 0) {
        print_indent(output, pos_x);
        str_cat(output, "└─ EMPTY\n");
        return;
    }

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

static void print_stmt_node_list(const List* stmt_node_list, Str* output, int pos_x)
{
    str_cat(output, "List<StmtNode>\n");

    if (!stmt_node_list || stmt_node_list->size == 0) {
        print_indent(output, pos_x);
        str_cat(output, "└─ EMPTY\n");
        return;
    }

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

static void print_fundef_node(const FundefNode* fundef_node, Str* output, int pos_x)
{
    if (!fundef_node) {
        print_node("FundefNode", "NULL", output, pos_x);
        return;
    }

    char buffer[256];
    str_cat(output, "FundefNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", fundef_node->name ? fundef_node->name : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ return_size: \"%s\"\n", fundef_node->return_size ? fundef_node->return_size : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ scope: \"%s\"\n", fundef_node->scope ? fundef_node->scope : "NULL");
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "├─ parameters: ");
    print_param_node_list(fundef_node->param_node_list, output, pos_x + 15);

    print_indent(output, pos_x);
    str_cat(output, "└─ block_node: ");
    print_block_node(fundef_node->block_node, output, pos_x + 15);
}

Str* print_nodelist(const List* node_list)
{
    if (!node_list) {
        return str_new("NULL\n");
    }

    Str* output = str_new("[\n");

    for (int i = 0; i < node_list->size; ++i) {
        FundefNode* fundef_node = (FundefNode*)node_list->items[i];
        print_indent(output, 4);
        print_fundef_node(fundef_node, output, 4);

        // Jump to the next line after each def node
        if (i < node_list->size - 1) {
            str_cat(output, "    ,\n");
        }
    }

    str_cat(output, "\n]");

    return output;
}
