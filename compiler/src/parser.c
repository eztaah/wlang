#include <stdlib.h> // calloc(), free()
#include <string.h> // strdup()
#include <stdio.h>  // sprintf()

#include "compiler.h"

typedef struct {
    const List* token_list;
    I32 token_list_size;
    I32 index;
    I32 index_next_token;
    Token current_token;
    Token next_token;
} Parser;

static Parser* parser_new(const List* token_list)
{
    Parser* parser = calloc(1, sizeof(Parser));
    parser->token_list = token_list;
    parser->token_list_size = token_list->size;
    parser->index = 0;
    parser->current_token = *((Token*)token_list->items[parser->index]);

    parser->index_next_token = 1;
    parser->next_token = *((Token*)token_list->items[parser->index_next_token]);

    return parser;
}

static Void parser_free(Parser* parser)
{
    free(parser);
}

// static Token parser_peek(Parser* parser, I32 offset)
// {
//     I32 peek_index = parser->index + offset;
//     if (peek_index < parser->token_list_size) {
//         return *((Token*)parser->token_list->items[peek_index]);
//     }

//     // return a TOKEN_EOF if out of bounds
//     Token eof_token;
//     eof_token.type = TOKEN_EOF;
//     return eof_token;
// }

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
        USER_PANIC("Expected token type %s but got %s", tokentype_to_string(expected_token_type), tokentype_to_string(parser->current_token.type));
    }
    return eat_next_token_any(parser);
}

static ExprNode* parse_expr(Parser* parser);

static ExprNode* parse_fun_call(Parser* parser)
{
    Str* final_name = str_new("");
    // handling @ 
    if (parser->current_token.type == TOKEN_AT) {
        eat_next_token(parser, TOKEN_AT);

        // handling name
        Token name_token = eat_next_token(parser, TOKEN_ID);
        Char* name = strdup(name_token.value);

        str_cat(final_name, name);
    }
    else {
        // handling name
        Token name_token = eat_next_token(parser, TOKEN_ID);
        Char* name = strdup(name_token.value);

        str_cat(final_name, "w__");
        str_cat(final_name, name);
    }

    eat_next_token(parser, TOKEN_LPAREN);

    // handling arguments
    List* expr_node_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expr_node_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            eat_next_token(parser, TOKEN_COMMA);
        }
    }

    eat_next_token(parser, TOKEN_RPAREN);

    return funcall_node_new(str_to_char(final_name), expr_node_list);
}

static ExprNode* parse_primary(Parser* parser)
{
    if (parser->current_token.type == TOKEN_NUM) {
        Token token_number = eat_next_token(parser, TOKEN_NUM);
        return num_node_new(token_number.value);
    }
    else if ((parser->current_token.type == TOKEN_AT) || (parser->current_token.type == TOKEN_ID && parser->next_token.type == TOKEN_LPAREN)) {
        ExprNode* funcall_node = parse_fun_call(parser);
        return funcall_node;
    }
    else if (parser->current_token.type == TOKEN_ID) {
        Token token_id = eat_next_token(parser, TOKEN_ID);
        return varref_node_new(token_id.value);
    }
    else if (parser->current_token.type == TOKEN_AMPERSAND) {
        eat_next_token(parser, TOKEN_AMPERSAND); // eat &
        Token token_id = eat_next_token(parser, TOKEN_ID);
        return varaddr_node_new(token_id.value);
    }
    else if (parser->current_token.type == TOKEN_LPAREN) {
        eat_next_token_any(parser); // Eat '('
        ExprNode* expr = parse_expr(parser);
        eat_next_token(parser, TOKEN_RPAREN); // Eat ')'
        return expr;
    }

    PANIC("Expected number or '('\n");
    return NULL;
}

static ExprNode* parse_unary(Parser* parser)
{
    if (parser->current_token.type == TOKEN_MINUS) {
        eat_next_token_any(parser); // Eat '-'
        ExprNode* operand = parse_unary(parser);
        return unarop_node_new(TOKEN_MINUS, operand);
    }
    return parse_primary(parser);
}

static ExprNode* parse_multiplicative(Parser* parser)
{
    ExprNode* left = parse_unary(parser);
    while (parser->current_token.type == TOKEN_MUL || parser->current_token.type == TOKEN_DIV) {
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_unary(parser);
        left = binop_node_new(left, op.type, right);
    }
    return left;
}

static ExprNode* parse_additive(Parser* parser)
{
    ExprNode* left = parse_multiplicative(parser);
    while (parser->current_token.type == TOKEN_PLUS || parser->current_token.type == TOKEN_MINUS) {
        Token op = eat_next_token_any(parser);
        ExprNode* right = parse_multiplicative(parser);
        left = binop_node_new(left, op.type, right);
    }
    return left;
}

static ExprNode* parse_expr(Parser* parser)
{
    return parse_additive(parser);
}

static StmtNode* parse_varass(Parser* parser)
{
    // handling name
    Token name_token = eat_next_token(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    // handling content
    eat_next_token(parser, TOKEN_EQUAL);
    ExprNode* value = parse_expr(parser);

    return varass_node_new(name, value);
}

static StmtNode* parse_ret(Parser* parser)
{
    eat_next_token(parser, TOKEN_RET);

    // handling expression
    if (parser->current_token.type != TOKEN_SEMI) {
        Bool is_empty = FALSE;
        ExprNode* expr_node = parse_expr(parser);
        return ret_node_new(is_empty, expr_node);
    }
    else {
        Bool is_empty = TRUE;
        ExprNode* expr_node = NULL;
        return ret_node_new(is_empty, expr_node);
    }
}

static StmtNode* parse_sysc(Parser* parser)
{
    // eat %
    eat_next_token(parser, TOKEN_PERCENT);

    // eat name
    eat_next_token(parser, TOKEN_ID);

    // handling arguments
    eat_next_token(parser, TOKEN_LPAREN);
    List* expr_node_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expr_node_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            eat_next_token(parser, TOKEN_COMMA);
        }
    }
    eat_next_token(parser, TOKEN_RPAREN);

    return sysc_node_new(expr_node_list);
}

static StmtNode* expr_to_stmt_node(ExprNode* expr_node)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }
    stmt_node->type = NODE_EXPR;
    stmt_node->expr_node = *expr_node;
    free(expr_node);
    return stmt_node;
}

static StmtNode* parse_stmt(Parser* parser)
{
    // parse return statement
    if (parser->current_token.type == TOKEN_RET) {
        StmtNode* ret_node = parse_ret(parser);
        eat_next_token(parser, TOKEN_SEMI);
        return ret_node;
    }

    // parser var assignement
    else if (parser->current_token.type == TOKEN_ID && parser->next_token.type == TOKEN_EQUAL) {
        StmtNode* varass_node = parse_varass(parser);
        eat_next_token(parser, TOKEN_SEMI);
        return varass_node;
    }

    // parse syscall
    else if ((parser->current_token.type == TOKEN_PERCENT && parser->next_token.type == TOKEN_ID && strcmp(parser->next_token.value, "sysc") == 0)) {
        StmtNode* sysc_node = parse_sysc(parser);
        eat_next_token(parser, TOKEN_SEMI);
        return sysc_node;
    }

    // parse expression
    else {
        StmtNode* expr_node = expr_to_stmt_node(parse_expr(parser));
        eat_next_token(parser, TOKEN_SEMI);
        return expr_node;
    }
}

static ParamNode* parse_fun_param(Parser* parser)
{
    // handling name
    Token name_token = eat_next_token(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    return param_node_new(name);
}

static CodeblockNode* parse_codeblock_node(Parser* parser)
{
    eat_next_token(parser, TOKEN_LBRACE);

    List* stmt_node_list = list_new(sizeof(StmtNode));
    while (parser->current_token.type != TOKEN_RBRACE) {
        StmtNode* stmt_node = parse_stmt(parser);
        list_push(stmt_node_list, stmt_node);
    }

    eat_next_token(parser, TOKEN_RBRACE);

    return codeblock_node_new(stmt_node_list);
}

static FundefNode* parse_fundef(Parser* parser)
{
    // Handle function scope
    Char scope[255];
    if (parser->current_token.type == TOKEN_GLB) {
        eat_next_token(parser, TOKEN_GLB);
        sprintf(scope, "global");
    }
    else {
        sprintf(scope, "local");
    }

    Token name_token = eat_next_token(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);
    Str* full_name = str_new("w__");
    str_cat(full_name, name);

    eat_next_token(parser, TOKEN_LPAREN);

    // handling parameters
    List* param_node_list = list_new(sizeof(ParamNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(param_node_list, parse_fun_param(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            eat_next_token(parser, TOKEN_COMMA);
        }
    }
    eat_next_token(parser, TOKEN_RPAREN);

    // handling codeblock
    CodeblockNode* codeblock_node = parse_codeblock_node(parser);

    return fundef_node_new(str_to_char(full_name), scope, param_node_list, codeblock_node);
}

List* parse(const List* token_list)
{
    print(MSG_STEP, "parsing...\n");

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
