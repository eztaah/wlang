#include "lib.h"
#include "node.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Str* print_tokenlist(const List* token_list)
{
    Str* output = str_new("[\n");

    for (I32 i = 0; i < token_list->size; i++) {
        const Token* token = (Token*)token_list->items[i];
        const Char* token_type_name = tokentype_to_string(token->type);

        Char temp[1024];
        snprintf(temp, sizeof(temp), "    {%s, \"%s\"},\n", token_type_name, token->value);
        str_cat(output, temp);

        if (token->type == TOKEN_END_STATEMENT) {
            str_cat(output, "\n");
        }
    }
    str_cat(output, "]");

    return output;
}

static Void print_indent(Str* output, I32 pos_x)
{
    for (I32 i = 0; i < pos_x; ++i) {
        str_cat(output, " ");
    }
}

static Void print_expression_node(ExprNode* node, Str* output, I32 pos_x)
{
    if (!node) {
        return;
    }

    Char buffer[256];

    switch (node->type) {
        case NODE_NUMBER:
            str_cat(output, "NumberNode\n");
            print_indent(output, pos_x);
            snprintf(buffer, sizeof(buffer), "└─ value: %s\n", node->number_node.value);
            str_cat(output, buffer);
            break;
        case NODE_BINOP:
            snprintf(buffer, sizeof(buffer), "BinopNode\n");
            str_cat(output, buffer);
            print_indent(output, pos_x);
            snprintf(buffer, sizeof(buffer), "├─ op: %s\n", tokentype_to_string(node->bin_op_node.op));
            str_cat(output, buffer);
            print_indent(output, pos_x);
            str_cat(output, "├─ left:  ");
            print_expression_node(node->bin_op_node.left, output, pos_x + 10);
            print_indent(output, pos_x);
            str_cat(output, "└─ right: ");
            print_expression_node(node->bin_op_node.right, output, pos_x + 10);
            break;
        default:
            UNREACHABLE();
            break;
    }
}

static Void print_var_decl_node(VarDeclNode* node, Str* output, I32 pos_x)
{
    Char buffer[256];    

    print_indent(output, pos_x);
    str_cat(output, "VarDeclNode\n");

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ type: \"%s\"\n", node->type);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    snprintf(buffer, sizeof(buffer), "├─ name: \"%s\"\n", node->name);
    str_cat(output, buffer);

    print_indent(output, pos_x);
    str_cat(output, "└─ value: ");
    print_expression_node(node->value, output, pos_x + 10);
}

static Void print_statement_node(StmtNode* node, Str* output, I32 pos_x)
{
    if (!node)
        return;

    switch (node->type) {
        case NODE_VAR_DECL:
            print_var_decl_node(&node->var_decl, output, pos_x);
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
        StmtNode* node = (StmtNode*)node_list->items[i];
        print_statement_node(node, output, 4);

        // Saute une ligne après chaque statement node
        if (i < node_list->size - 1) {
            str_cat(output, "    ,\n");
        }
    }

    str_cat(output, "\n]");

    return output;
}
