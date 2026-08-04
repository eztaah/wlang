#include <stdio.h>  // sprintf()
#include <stdlib.h> // calloc(), free()
#include <string.h> // strdup()

#include "compiler.h"

typedef struct {
    const List* token_list;
    int token_list_size;
    int index;
    int index_next_token;
    Token current_token;
    Token next_token;
} Parser;

static Parser* parser_new(const List* token_list)
{
    Parser* parser = safe_calloc(1, sizeof(Parser));
    parser->token_list = token_list;
    parser->token_list_size = token_list->size;
    parser->index = 0;
    parser->current_token = *((Token*)token_list->items[parser->index]);

    parser->index_next_token = 1;
    parser->next_token = *((Token*)token_list->items[parser->index_next_token]);

    return parser;
}

static void parser_free(Parser* parser)
{
    free(parser);
}

static Token eat_next_token_any(Parser* parser)
{
    Token current_token = parser->current_token;
    parser->index++;
    if (parser->index < parser->token_list_size) {
        parser->current_token = *((Token*)parser->token_list->items[parser->index]);
    }
    parser->index_next_token = MIN(parser->index + 1, parser->token_list_size - 1);
    parser->next_token = *((Token*)parser->token_list->items[parser->index_next_token]);

    return current_token;
}

static Token eat_next_token(Parser* parser, TokenType expected_token_type)
{
    if (parser->current_token.type != expected_token_type) {
        USER_PANIC(current_filename, parser->current_token.line, "expected token type %s but got %s", tokentype_to_string(expected_token_type), tokentype_to_string(parser->current_token.type));
    }
    return eat_next_token_any(parser);
}

static ExprNode* parse_expr(Parser* parser);

static ExprNode* parse_fun_call(Parser* parser)
{
    Str* final_name = str_new("");
    int line = parser->current_token.line;

    // handling @
    if (parser->current_token.type == TOKEN_AT) {
        eat_next_token(parser, TOKEN_AT);

        // handling name
        Token name_token = eat_next_token(parser, TOKEN_ID);
        char* name = strdup(name_token.value);

        str_cat(final_name, name);
    }
    else {
        // handling name
        Token name_token = eat_next_token(parser, TOKEN_ID);
        char* name = strdup(name_token.value);

        str_cat(final_name, "w__");
        str_cat(final_name, name);
    }

    eat_next_token(parser, TOKEN_LPAREN);

    // handling arguments
    List* expr_node_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expr_node_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameter (and there is no comma after)
            eat_next_token(parser, TOKEN_COMMA);
        }
    }

    eat_next_token(parser, TOKEN_RPAREN);
    ExprNode* node = funcall_node_new(str_to_char(final_name), expr_node_list, line);
    str_free(final_name);
    return node;
}

static ExprNode* parse_sysc(Parser* parser)
{
    int line = parser->current_token.line;

    // eat %
    eat_next_token(parser, TOKEN_PERCENT);

    // eat name
    eat_next_token(parser, TOKEN_ID);

    // handling arguments
    eat_next_token(parser, TOKEN_LPAREN);
    List* expr_node_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expr_node_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameter (and there is no comma after)
            eat_next_token(parser, TOKEN_COMMA);
        }
    }
    eat_next_token(parser, TOKEN_RPAREN);

    return sysc_node_new(expr_node_list, line);
}

static ExprNode* parse_primary(Parser* parser)
{
    Token token = parser->current_token;
    int line = token.line;

    switch (token.type) {
        case TOKEN_NUM: {
            eat_next_token_any(parser);
            return num_node_new(token.value, line);
        }
        case TOKEN_AT:
        case TOKEN_ID: {
            if (token.type == TOKEN_AT || parser->next_token.type == TOKEN_LPAREN) {
                return parse_fun_call(parser);
            }
            eat_next_token_any(parser);
            return varref_node_new(token.value, line);
        }
        case TOKEN_PERCENT: {
            if (parser->next_token.type == TOKEN_ID && strcmp(parser->next_token.value, "sysc") == 0) {
                return parse_sysc(parser);
            }
            break;
        }
        case TOKEN_AMPERSAND: {
            eat_next_token_any(parser);
            Token id_token = eat_next_token(parser, TOKEN_ID);
            return varaddr_node_new(id_token.value, line);
        }
        case TOKEN_LPAREN: {
            eat_next_token_any(parser); // Eat '('
            ExprNode* expr = parse_expr(parser);
            eat_next_token(parser, TOKEN_RPAREN); // Eat ')'
            return expr;
        }
        default:
            break;
    }

    USER_PANIC(current_filename, parser->current_token.line, "expected primary expression (number, identifier, or parentheses)\n");
    return NULL;
}

static ExprNode* parse_unary(Parser* parser)
{
    int line = parser->current_token.line;

    if (parser->current_token.type == TOKEN_MINUS) {
        eat_next_token_any(parser); // Eat '-'
        ExprNode* operand = parse_unary(parser);
        return unarop_node_new(TOKEN_MINUS, operand, line);
    }
    else if (parser->current_token.type == TOKEN_TILDE) {
        eat_next_token_any(parser); // Eat '~'
        ExprNode* operand = parse_unary(parser);
        return unarop_node_new(TOKEN_TILDE, operand, line);
    }
    else if (parser->current_token.type == TOKEN_CARET) {
        eat_next_token_any(parser); // Eat '^'
        ExprNode* operand = parse_unary(parser);
        return unarop_node_new(TOKEN_CARET, operand, line);
    }
    return parse_primary(parser);
}

static ExprNode* parse_multiplicative(Parser* parser)
{
    ExprNode* left = parse_unary(parser);
    while (parser->current_token.type == TOKEN_MUL || parser->current_token.type == TOKEN_DIV || parser->current_token.type == TOKEN_PERCENT) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_unary(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_additive(Parser* parser)
{
    ExprNode* left = parse_multiplicative(parser);
    while (parser->current_token.type == TOKEN_PLUS || parser->current_token.type == TOKEN_MINUS) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_multiplicative(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_shift(Parser* parser)
{
    ExprNode* left = parse_additive(parser);
    while (parser->current_token.type == TOKEN_LEFTSHIFT || parser->current_token.type == TOKEN_RIGHTSHIFT) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_additive(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_relational(Parser* parser)
{
    ExprNode* left = parse_shift(parser);
    while (parser->current_token.type == TOKEN_LESSTHAN || parser->current_token.type == TOKEN_GREATERTHAN || parser->current_token.type == TOKEN_LESSTHAN_EQ || parser->current_token.type == TOKEN_GREATERTHAN_EQ) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_shift(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_equality(Parser* parser)
{
    ExprNode* left = parse_relational(parser);
    while (parser->current_token.type == TOKEN_EQUAL_EQUAL || parser->current_token.type == TOKEN_NOT_EQUAL) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_relational(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_bitwise_and(Parser* parser)
{
    ExprNode* left = parse_equality(parser);
    while (parser->current_token.type == TOKEN_AMPERSAND) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_equality(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_bitwise_xor(Parser* parser)
{
    ExprNode* left = parse_bitwise_and(parser);
    while (parser->current_token.type == TOKEN_HASH) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_bitwise_and(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_bitwise_or(Parser* parser)
{
    ExprNode* left = parse_bitwise_xor(parser);
    while (parser->current_token.type == TOKEN_PIPE) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_bitwise_xor(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_logical_and(Parser* parser)
{
    ExprNode* left = parse_bitwise_or(parser);
    while (parser->current_token.type == TOKEN_AND) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_bitwise_or(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_logical_or(Parser* parser)
{
    ExprNode* left = parse_logical_and(parser);
    while (parser->current_token.type == TOKEN_OR) {
        int line = parser->current_token.line;
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_logical_and(parser);
        left = binop_node_new(left, op.type, right, line);
    }
    return left;
}

static ExprNode* parse_expr(Parser* parser)
{
    return parse_logical_or(parser);
}

static StmtNode* parse_ret(Parser* parser)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_RET);

    // handling expression
    if (parser->current_token.type != TOKEN_SEMI) {
        ExprNode* expr_node = parse_expr(parser);
        eat_next_token(parser, TOKEN_SEMI);
        return ret_node_new(expr_node, line);
    }
    else {
        ExprNode* expr_node = NULL;
        eat_next_token(parser, TOKEN_SEMI);
        return ret_node_new(expr_node, line);
    }
}

static StmtNode* expr_to_stmt_node(ExprNode* expr_node, int line)
{
    StmtNode* stmt_node = (StmtNode*)safe_malloc(sizeof(StmtNode));
    stmt_node->type = NODE_EXPR;
    stmt_node->line = line;
    stmt_node->expr_node = *expr_node;
    free(expr_node);
    return stmt_node;
}

static StmtNode* parse_stmt(Parser* parser);

static BlockNode* parse_block_node(Parser* parser)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_LBRACE);

    List* stmt_node_list = list_new(sizeof(StmtNode));
    while (parser->current_token.type != TOKEN_RBRACE) {
        StmtNode* stmt_node = parse_stmt(parser);
        list_push(stmt_node_list, stmt_node);
    }

    eat_next_token(parser, TOKEN_RBRACE);

    return block_node_new(stmt_node_list, line);
}

static StmtNode* parse_if_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_IF);

    // Parse condition
    eat_next_token(parser, TOKEN_LPAREN);
    ExprNode* cond_node = parse_expr(parser);
    eat_next_token(parser, TOKEN_RPAREN);

    // Parse true block
    BlockNode* true_block = parse_block_node(parser);

    // Parse false block (optional)
    BlockNode* false_block = NULL;
    if (parser->current_token.type == TOKEN_ELSE) {
        eat_next_token(parser, TOKEN_ELSE);
        false_block = parse_block_node(parser);
    }

    return if_node_new(cond_node, true_block, false_block, line);
}

static StmtNode* parse_loop_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_LOOP);

    BlockNode* block_node = parse_block_node(parser);
    return loop_node_new(block_node, line);
}

static StmtNode* parse_break_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_BREAK);
    eat_next_token(parser, TOKEN_SEMI);
    return break_node_new(line);
}

static StmtNode* parse_vardec_stmt(Parser* parser)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_LESSTHAN);
    Token size_token = eat_next_token(parser, TOKEN_NUM);
    char* size = strdup(size_token.value);
    eat_next_token(parser, TOKEN_GREATERTHAN);

    Token name_token = eat_next_token(parser, TOKEN_ID);
    char* name = strdup(name_token.value);

    // handle variable declaration without assignment
    if (parser->current_token.type == TOKEN_SEMI) {
        eat_next_token(parser, TOKEN_SEMI);
        return vardec_node_new(size, name, NULL, line);
    }

    // handle variable declaration with assignment
    else if (parser->current_token.type == TOKEN_EQUAL) {
        eat_next_token(parser, TOKEN_EQUAL);
        ExprNode* expr_node = parse_expr(parser);
        eat_next_token(parser, TOKEN_SEMI);
        return vardec_node_new(size, name, expr_node, line);
    }

    // handle array declarations (with and without assignment)
    else if (parser->current_token.type == TOKEN_LBRACKET) {
        eat_next_token(parser, TOKEN_LBRACKET);
        Token array_size_token = eat_next_token(parser, TOKEN_NUM);
        char* array_size = strdup(array_size_token.value);
        eat_next_token(parser, TOKEN_RBRACKET);

        // with assignment
        if (parser->current_token.type == TOKEN_EQUAL) {
            eat_next_token(parser, TOKEN_EQUAL);

            // handling arguments
            eat_next_token(parser, TOKEN_LBRACKET);
            List* expr_node_list = list_new(sizeof(ExprNode));
            while (parser->current_token.type != TOKEN_RBRACKET) {
                list_push(expr_node_list, parse_expr(parser));
                if (parser->current_token.type == TOKEN_COMMA) {
                    eat_next_token(parser, TOKEN_COMMA);
                }
            }
            eat_next_token(parser, TOKEN_RBRACKET);

            eat_next_token(parser, TOKEN_SEMI);
            return arraydec_node_new(size, name, array_size, expr_node_list, line);
        }

        // without assignment
        else if (parser->current_token.type == TOKEN_SEMI) {
            eat_next_token(parser, TOKEN_SEMI);
            return arraydec_node_new(size, name, array_size, NULL, line);
        }
        else {
            UNREACHABLE();
            return NULL;
        }
    }

    UNREACHABLE();
    return NULL;
}

static StmtNode* parse_ass_stmt(Parser* parser, ExprNode* expr_node1)
{
    int line = parser->current_token.line;
    eat_next_token(parser, TOKEN_EQUAL);

    // parse value of the var assignment
    ExprNode* expr_node2 = parse_expr(parser);
    eat_next_token(parser, TOKEN_SEMI);

    return ass_node_new(expr_node1, expr_node2, line);
}

static StmtNode* parse_stmt(Parser* parser)
{
    switch (parser->current_token.type) {
        case TOKEN_RET:
            return parse_ret(parser);

        case TOKEN_IF:
            return parse_if_stmt(parser);

        case TOKEN_LOOP:
            return parse_loop_stmt(parser);

        case TOKEN_BREAK:
            return parse_break_stmt(parser);

        case TOKEN_LESSTHAN:
            return parse_vardec_stmt(parser);

        default: {
            int line = parser->current_token.line;
            ExprNode* expr_node = parse_expr(parser);

            if (parser->current_token.type == TOKEN_EQUAL) {
                return parse_ass_stmt(parser, expr_node);
            }

            StmtNode* stmt_node = expr_to_stmt_node(expr_node, line);
            eat_next_token(parser, TOKEN_SEMI);
            return stmt_node;
        }
    }

    UNREACHABLE();
    return NULL;
}

static ParamNode* parse_fun_param(Parser* parser)
{
    int line = parser->current_token.line;

    // handling size
    eat_next_token(parser, TOKEN_LESSTHAN);
    Token size_token = eat_next_token(parser, TOKEN_NUM);
    char* size = strdup(size_token.value);
    eat_next_token(parser, TOKEN_GREATERTHAN);

    // handling name
    Token name_token = eat_next_token(parser, TOKEN_ID);
    char* name = strdup(name_token.value);

    return param_node_new(name, size, line);
}

static FundefNode* parse_fundef(Parser* parser)
{
    int line = parser->current_token.line;

    // Handle function scope
    char scope[255];
    if (parser->current_token.type == TOKEN_GLB) {
        eat_next_token(parser, TOKEN_GLB);
        sprintf(scope, "global");
    }
    else {
        sprintf(scope, "local");
    }

    // handle return size
    char* return_size = NULL;
    if (parser->current_token.type == TOKEN_LESSTHAN) {
        eat_next_token(parser, TOKEN_LESSTHAN);
        Token size_token = eat_next_token(parser, TOKEN_NUM);
        return_size = strdup(size_token.value);
        eat_next_token(parser, TOKEN_GREATERTHAN);
    }

    // handle name
    Token name_token = eat_next_token(parser, TOKEN_ID);
    char* name = strdup(name_token.value);
    Str* full_name = str_new("");

    if (strcmp(name, "main") == 0 && no_start_fun) {
        str_cat(full_name, "main");
    }
    else {
        str_cat(full_name, "w__");
        str_cat(full_name, name);
    }

    eat_next_token(parser, TOKEN_LPAREN);

    // handling parameters
    List* param_node_list = list_new(sizeof(ParamNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(param_node_list, parse_fun_param(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameter (and there is no comma after)
            eat_next_token(parser, TOKEN_COMMA);
        }
    }
    eat_next_token(parser, TOKEN_RPAREN);

    // handling block_node
    BlockNode* block_node = parse_block_node(parser);

    FundefNode* fundef_node = fundef_node_new(str_to_char(full_name), return_size, scope, param_node_list, block_node, line);
    str_free(full_name);
    free(name);
    return fundef_node;
}

List* parse(const List* token_list)
{
    Parser* parser = parser_new(token_list);

    List* fundef_node_list = list_new(sizeof(FundefNode));
    while (parser->index < parser->token_list_size) {
        if (parser->current_token.type == TOKEN_EOF) {
            break;
        }
        FundefNode* fundef_node = parse_fundef(parser);
        list_push(fundef_node_list, fundef_node);
    }

    parser_free(parser);
    return fundef_node_list;
}
