#include <stdlib.h> // calloc(), free()
#include <string.h> // strdup()

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

static Token parser_eat(Parser* parser)
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

static Token parser_eat_assumes(Parser* parser, TokenType expected_token_type)
{
    Token token = parser->current_token;
    if (token.type != expected_token_type) {
        PANIC("Error: expected token type %d but got %d\n", expected_token_type, token.type);
        // print(MSG_ERROR,"Error: expected token type %d but got %d\n", expected_token_type, token.type); // we don't want to handle it the the PANIC macro as it is an error that can be triggered by the user
        // exit(EXIT_FAILURE);
    }
    return parser_eat(parser);
}

static ExprNode* parse_expr(Parser* parser);

static ExprNode* parse_fun_call(Parser* parser)
{
    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_LPAREN);

    // handling arguments
    List* expr_node_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expr_node_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            parser_eat_assumes(parser, TOKEN_COMMA);
        }
    }

    parser_eat_assumes(parser, TOKEN_RPAREN);

    return funcall_node_new(name, expr_node_list);
}

static ExprNode* parse_primary(Parser* parser)
{
    Token token = parser->current_token;
    if (token.type == TOKEN_INT) {
        Token token_number = parser_eat_assumes(parser, TOKEN_INT);
        return num_node_new(token_number.value);
    }
    else if ((parser->current_token.type == TOKEN_ID && parser->next_token.type == TOKEN_LPAREN)) {
        ExprNode* funcall_node = parse_fun_call(parser);
        return funcall_node;
    }
    else if (token.type == TOKEN_ID) {
        Token token_id = parser_eat_assumes(parser, TOKEN_ID);
        return varref_node_new(token_id.value);
    }
    else if (token.type == TOKEN_AMPERSAND) {
        parser_eat_assumes(parser, TOKEN_AMPERSAND); // eat &
        Token token_id = parser_eat_assumes(parser, TOKEN_ID);
        return varaddr_node_new(token_id.value);
    }
    else if (token.type == TOKEN_LPAREN) {
        parser_eat(parser); // Eat '('
        ExprNode* expr = parse_expr(parser);
        parser_eat_assumes(parser, TOKEN_RPAREN); // Eat ')'
        return expr;
    }

    PANIC("Error: expected number or '('\n");
    // print(MSG_ERROR,"Error: expected number or '('\n");
    // exit(EXIT_FAILURE);
    return NULL;
}

static ExprNode* parse_unary(Parser* parser)
{
    Token token = parser->current_token;
    if (token.type == TOKEN_MINUS) {
        parser_eat(parser); // Eat '-'
        ExprNode* operand = parse_unary(parser);
        return unarop_node_new(TOKEN_MINUS, operand);
    }
    return parse_primary(parser);
}

static ExprNode* parse_multiplicative(Parser* parser)
{
    ExprNode* left = parse_unary(parser);
    while (parser->current_token.type == TOKEN_MUL || parser->current_token.type == TOKEN_DIV) {
        Token op = parser_eat(parser);
        ExprNode* right = parse_unary(parser);
        left = binop_node_new(left, op.type, right);
    }
    return left;
}

static ExprNode* parse_additive(Parser* parser)
{
    ExprNode* left = parse_multiplicative(parser);
    while (parser->current_token.type == TOKEN_PLUS || parser->current_token.type == TOKEN_MINUS) {
        Token op = parser_eat(parser);
        ExprNode* right = parse_multiplicative(parser);
        left = binop_node_new(left, op.type, right);
    }
    return left;
}

static ExprNode* parse_expr(Parser* parser)
{
    return parse_additive(parser);
}

static StmtNode* parse_vardef(Parser* parser)
{
    // handling cst and var keywords
    Token mut_token = parser_eat_assumes(parser, TOKEN_MUT);
    Char* mut = strdup(mut_token.value);

    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_COLON);

    // handling type
    Token type_token = parser_eat_assumes(parser, TOKEN_TYPE);
    Char* type = strdup(type_token.value);

    // handling content
    parser_eat_assumes(parser, TOKEN_EQUAL);
    ExprNode* value = parse_expr(parser);

    return vardef_node_new(mut, type, name, value);
}

static StmtNode* parse_varmod(Parser* parser)
{
    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    // handling content
    parser_eat_assumes(parser, TOKEN_EQUAL);
    ExprNode* value = parse_expr(parser);

    return varmod_node_new(name, value);
}

static StmtNode* parse_return(Parser* parser)
{
    parser_eat_assumes(parser, TOKEN_RETURN);

    // handling expression
    if (parser->current_token.type != TOKEN_END_INSTR) {
        Bool is_empty = FALSE;
        ExprNode* expr_node = parse_expr(parser);
        return return_node_new(is_empty, expr_node);
    }
    else {
        Bool is_empty = TRUE;
        ExprNode* expr_node = NULL;
        return return_node_new(is_empty, expr_node);
    }
}

static StmtNode* parse_syscall(Parser* parser)
{
    // eat @
    parser_eat_assumes(parser, TOKEN_AT);

    // eat name
    parser_eat_assumes(parser, TOKEN_ID);

    parser_eat_assumes(parser, TOKEN_LPAREN);

    // handling arguments
    List* expr_node_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expr_node_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            parser_eat_assumes(parser, TOKEN_COMMA);
        }
    }
    parser_eat_assumes(parser, TOKEN_RPAREN);

    return syscall_node_new(expr_node_list);
}

StmtNode* expr_to_stmt_node(ExprNode* expr_node)
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
    // parser var declaration
    if (parser->current_token.type == TOKEN_MUT) {
        StmtNode* vardef_node = parse_vardef(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return vardef_node;
    }
    // parser var modif
    else if (parser->current_token.type == TOKEN_ID && parser->next_token.type == TOKEN_EQUAL) {
        StmtNode* varmod_node = parse_varmod(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return varmod_node;
    }
    // parse return statement
    else if (parser->current_token.type == TOKEN_RETURN) {
        StmtNode* return_node = parse_return(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return return_node;
    }
    else if ((parser->current_token.type == TOKEN_AT && parser->next_token.type == TOKEN_ID && strcmp(parser->next_token.value, "syscall") == 0)) {
        StmtNode* syscall_node = parse_syscall(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return syscall_node;
    }
    else {
        StmtNode* expr_node = expr_to_stmt_node(parse_expr(parser));
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return expr_node;

        // UNREACHABLE();
        // return NULL;
    }
}

static ParamNode* parse_fun_param(Parser* parser)
{
    // handling cst and var keywords
    Token mut_token = parser_eat_assumes(parser, TOKEN_MUT);
    Char* mut = strdup(mut_token.value);

    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_COLON);

    // handling type
    Token type_token = parser_eat_assumes(parser, TOKEN_TYPE);
    Char* type = strdup(type_token.value);

    return param_node_new(mut, name, type);
}

static CodeblockNode* parse_codeblock_node(Parser* parser)
{
    parser_eat_assumes(parser, TOKEN_LBRACE);

    List* stmt_node_list = list_new(sizeof(StmtNode));
    while (parser->current_token.type != TOKEN_RBRACE) {
        StmtNode* stmt_node = parse_stmt(parser);
        list_push(stmt_node_list, stmt_node);
    }

    parser_eat_assumes(parser, TOKEN_RBRACE);

    return codeblock_node_new(stmt_node_list);
}

static DefNode* parse_fundef(Parser* parser)
{
    // eat fun keyword
    parser_eat_assumes(parser, TOKEN_FUN);

    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_LPAREN);

    // handling parameters
    List* param_node_list = list_new(sizeof(ParamNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(param_node_list, parse_fun_param(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            parser_eat_assumes(parser, TOKEN_COMMA);
        }
    }
    parser_eat_assumes(parser, TOKEN_RPAREN);

    parser_eat_assumes(parser, TOKEN_COLON);
    Token token_type = parser_eat_assumes(parser, TOKEN_TYPE);
    Char* type = strdup(token_type.value);

    // handling codeblock
    CodeblockNode* codeblock_node = parse_codeblock_node(parser);

    return fundef_node_new(name, type, param_node_list, codeblock_node);
}

static DefNode* parse_start_def(Parser* parser)
{
    // eat _start keyword
    parser_eat_assumes(parser, TOKEN_ID);

    // handling codeblock
    CodeblockNode* codeblock_node = parse_codeblock_node(parser);

    return startdef_node_new(codeblock_node);
}

static DefNode* parse_def(Parser* parser)
{
    if (parser->current_token.type == TOKEN_FUN) {
        DefNode* fundef_node = parse_fundef(parser);
        return fundef_node;
    }
    else if (parser->current_token.type == TOKEN_ID && strcmp(parser->current_token.value, "_start") == 0) {
        DefNode* startdef_node = parse_start_def(parser);
        return startdef_node;
    }
    else {
        UNREACHABLE();
        return NULL;
    }
}

List* parse(const List* token_list)
{
    print(MSG_STEP, "parsing...\n");

    Parser* parser = parser_new(token_list);

    List* stmt_list = list_new(sizeof(DefNode));
    while (parser->index < parser->token_list_size) {
        if (parser->current_token.type == TOKEN_EOF) {
            break;
        }
        DefNode* def_node = parse_def(parser);
        list_push(stmt_list, def_node);
    }

    parser_free(parser);
    return stmt_list;
}
