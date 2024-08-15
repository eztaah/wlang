#include <stdio.h> // snprintf()

#include "lib.h"
#include "node.h"

static Void print_indent(Str* output, I32 pos_x)
{
    for (I32 i = 0; i < pos_x; ++i) {
        str_cat(output, " ");
    }
}

static Void print_num_node(const NumNode* num_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "NumNode\n");
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ value: %s\n", num_node->value);
    str_cat(output, buffer);
}

static Void print_varref_node(const VarrefNode* varref_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "VarrefNode\n");
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ name: %s\n", varref_node->name);
    str_cat(output, buffer);
}

static Void print_expr_node(const ExprNode* node, Str* output, I32 pos_x);

static Void print_addrderef_node(const AddrderefNode* addrderef_node, Str* output, I32 pos_x)
{
    str_cat(output, "AddrderefNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ expr: ");
    print_expr_node(addrderef_node->expr, output, pos_x + 10);
}

static Void print_varaddr_node(const VaraddrNode* varaddr_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "VaraddrNode\n");
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ name: %s\n", varaddr_node->name);
    str_cat(output, buffer);
}

static Void print_binop_node(const BinopNode* binop_node, Str* output, I32 pos_x)
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

static Void print_unarop_node(const UnaropNode* unarop_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    snprintf(buffer, sizeof(buffer), "UnaryopNode\n");
    str_cat(output, buffer);
    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ op: %s\n", tokentype_to_string(unarop_node->op));
    str_cat(output, buffer);
    print_indent(output, pos_x);
    str_cat(output, "└─ operand: ");
    print_expr_node(unarop_node->operand, output, pos_x + 12);
}

static Void print_expr_node_list(const List* expr_nodelist, Str* output, I32 pos_x)
{
    str_cat(output, "List<ExprNode>\n");

    if (expr_nodelist->size > 0) {
        for (I32 i = 0; i < expr_nodelist->size - 1; i++) {
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
    else {
        print_indent(output, pos_x);
        str_cat(output, "└─ EMPTY\n");
    }
}

static Void print_funcall_node(const FuncallNode* funcall_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "FuncallNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", funcall_node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ args: ");
    print_expr_node_list(funcall_node->expr_node_list, output, pos_x + 9);
}

static Void print_sysc_node(const SyscNode* sysc_node, Str* output, I32 pos_x)
{
    str_cat(output, "SyscNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ args: ");
    print_expr_node_list(sysc_node->expr_node_list, output, pos_x + 9);
}

static Void print_expr_node(const ExprNode* node, Str* output, I32 pos_x)
{
    switch (node->type) {
        case NODE_NUM:
            print_num_node(&node->num_node, output, pos_x);
            break;

        case NODE_VARREF:
            print_varref_node(&node->varref_node, output, pos_x);
            break;

        case NODE_ADDRDEREF:
            print_addrderef_node(&node->addrderef_node, output, pos_x);
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
            UNREACHABLE();
            break;
    }
}

static Void print_varass_node(const VarAssNode* node, Str* output, I32 pos_x)
{
    str_cat(output, "VarAssNode\n");

    print_indent(output, pos_x);
    str_cat(output, "├─ lvalue: ");
    print_expr_node(node->lvalue, output, pos_x + 11);

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expr_node(node->value, output, pos_x + 10);
}

static Void print_ret_node(const RetNode* node, Str* output, I32 pos_x)
{
    str_cat(output, "RetNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    
    print_expr_node(node->expr_node, output, pos_x + 10);
}

static Void print_stmt_node(const StmtNode* stmt_node, Str* output, I32 pos_x)
{
    switch (stmt_node->type) {
        case NODE_VARASS:
            print_varass_node(&stmt_node->varass_node, output, pos_x);
            break;

        case NODE_RET:
            print_ret_node(&stmt_node->ret_node, output, pos_x);
            break;

        case NODE_EXPR:
            print_expr_node(&stmt_node->expr_node, output, pos_x);
            break;

        default:
            UNREACHABLE();
            break;
    }
}

static Void print_param_node(const ParamNode* node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "ParamNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "└─ name: \"%s\"\n", node->name);
    str_cat(output, buffer);
}

static Void print_param_node_list(const List* param_node_list, Str* output, I32 pos_x)
{
    str_cat(output, "List<ParamNode>\n");

    if (param_node_list->size > 0) {
        for (I32 i = 0; i < param_node_list->size - 1; i++) {
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
    else {
        print_indent(output, pos_x);
        str_cat(output, "└─ EMPTY\n");
    }
}

static Void print_stmt_node_list(const List* stmt_node_list, Str* output, I32 pos_x)
{
    str_cat(output, "List<StmtNode>\n");

    if (stmt_node_list->size > 0) {
        for (I32 i = 0; i < stmt_node_list->size - 1; i++) {
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
        str_cat(output, "└─ EMPTY\n");
    }
}

static Void print_codeblock_node(const CodeblockNode* codeblock_node, Str* output, I32 pos_x)
{
    str_cat(output, "CodeblockNode\n");

    print_indent(output, pos_x);
    str_cat(output, "└─ content: ");
    print_stmt_node_list(codeblock_node->stmt_node_list, output, pos_x + 12);
}

static Void print_fundef_node(const FundefNode* fundef_node, Str* output, I32 pos_x)
{
    Char buffer[256];

    str_cat(output, "FundefNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", fundef_node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ scope: \"%s\"\n", fundef_node->scope);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "├─ parameters: ");
    print_param_node_list(fundef_node->param_node_list, output, pos_x + 15);

    print_indent(output, pos_x);
    str_cat(output, "└─ codeblock_node: ");
    print_codeblock_node(fundef_node->codeblock_node, output, pos_x + 15);
}

Str* print_nodelist(const List* node_list)
{
    Str* output = str_new("[\n");

    for (I32 i = 0; i < node_list->size; ++i) {
        FundefNode* fundef_node = (FundefNode*)node_list->items[i];
        print_indent(output, 4);
        print_fundef_node(fundef_node, output, 4);

        // jump to the next line after each def node
        if (i < node_list->size - 1) {
            str_cat(output, "    ,\n");
        }
    }

    str_cat(output, "\n]");

    return output;
}
