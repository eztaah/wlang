#include "compiler.h"
#include "node.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // TODO: Verify if we need to free the parser->token_list
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
        // printf("Error: expected token type %d but got %d\n", expected_token_type, token.type); // we don't want to handle it the the PANIC macro as it is an error that can be triggered by the user
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
    List* expression_nodes_list = list_new(sizeof(ExprNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(expression_nodes_list, parse_expr(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            parser_eat_assumes(parser, TOKEN_COMMA);
        }
    }

    parser_eat_assumes(parser, TOKEN_RPAREN);

    return fun_call_node_new(name, expression_nodes_list);
}

static ExprNode* parse_primary(Parser* parser)
{
    Token token = parser->current_token;
    if (token.type == TOKEN_INT) {
        Token token_number = parser_eat_assumes(parser, TOKEN_INT);
        return number_node_new(token_number.value);
    }
    else if ((parser->current_token.type == TOKEN_ID && parser->next_token.type == TOKEN_LPAREN)) {
        ExprNode* fun_call_node = parse_fun_call(parser);
        return fun_call_node;
    }
    else if (token.type == TOKEN_ID) {
        Token token_id = parser_eat_assumes(parser, TOKEN_ID);
        return var_ref_node_new(token_id.value);
    }
    else if (token.type == TOKEN_LPAREN) {
        parser_eat(parser); // Eat '('
        ExprNode* expr = parse_expr(parser);
        parser_eat_assumes(parser, TOKEN_RPAREN); // Eat ')'
        return expr;
    }

    PANIC("Error: expected number or '('\n");
    // printf("Error: expected number or '('\n");
    // exit(EXIT_FAILURE);
    return NULL;
}

static ExprNode* parse_unary(Parser* parser)
{
    Token token = parser->current_token;
    if (token.type == TOKEN_MINUS) {
        parser_eat(parser); // Eat '-'
        ExprNode* operand = parse_unary(parser);
        return unaryop_node_new(TOKEN_MINUS, operand);
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

static InstrNode* parse_var_def(Parser* parser)
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

    return var_def_node_new(mut, type, name, value);
}

static InstrNode* parse_var_modif(Parser* parser)
{
    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    // handling content
    parser_eat_assumes(parser, TOKEN_EQUAL);
    ExprNode* value = parse_expr(parser);

    return var_modif_node_new(name, value);
}

static InstrNode* parse_return(Parser* parser)
{
    parser_eat_assumes(parser, TOKEN_RETURN);

    // handling expression
    ExprNode* expr_node = parse_expr(parser);

    return return_node_new(expr_node);
}

static InstrNode* parse_syscallwrite(Parser* parser)
{
    // eat @ 
    parser_eat_assumes(parser, TOKEN_AT);

    // eat name
    parser_eat_assumes(parser, TOKEN_ID);

    // handle the argument given
    parser_eat_assumes(parser, TOKEN_LPAREN);
    parser_eat_assumes(parser, TOKEN_AMPERSAND);

    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* char_location_ptr_name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_RPAREN);

    return syscallwrite_node_new(char_location_ptr_name);
}

InstrNode* expr_to_instr_node(ExprNode* expr_node)
{
    InstrNode* instr_node = (InstrNode*)malloc(sizeof(InstrNode));
    if (!instr_node) {
        PANIC("failed to allocate memory");
    }
    instr_node->type = NODE_EXPR;
    instr_node->expr_node = *expr_node;
    free(expr_node);
    return instr_node;
}

static InstrNode* parse_instr(Parser* parser)
{
    // parser var declaration
    if (parser->current_token.type == TOKEN_MUT) {
        InstrNode* var_def_node = parse_var_def(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return var_def_node;
    }
    // parser var modif
    else if (parser->current_token.type == TOKEN_ID && parser->next_token.type == TOKEN_EQUAL) {
        InstrNode* var_modif_node = parse_var_modif(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return var_modif_node;
    }
    // parse return statement
    else if (parser->current_token.type == TOKEN_RETURN) {
        InstrNode* return_node = parse_return(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return return_node;
    }
    else if ((parser->current_token.type == TOKEN_AT && parser->next_token.type == TOKEN_ID)) {
        InstrNode* syscallwrite_node = parse_syscallwrite(parser);
        parser_eat_assumes(parser, TOKEN_END_INSTR);
        return syscallwrite_node;
    }
    else {
        InstrNode* expr_node = expr_to_instr_node(parse_expr(parser));
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

static StmtNode* parse_stmt(Parser* parser);
static CodeblockNode* parse_code_block(Parser* parser)
{
    parser_eat_assumes(parser, TOKEN_LBRACE);

    List* stmt_nodes_list = list_new(sizeof(StmtNode));
    while (parser->current_token.type != TOKEN_RBRACE) {
        StmtNode* stmt = parse_stmt(parser);
        list_push(stmt_nodes_list, stmt);
    }

    parser_eat_assumes(parser, TOKEN_RBRACE);

    return code_block_new(stmt_nodes_list);
}

static StmtNode* parse_fun_def(Parser* parser)
{
    // eat fun keyword
    parser_eat_assumes(parser, TOKEN_FUN);

    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    Char* name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_LPAREN);

    // handling parameters
    List* param_nodes_list = list_new(sizeof(ParamNode));
    while (parser->current_token.type != TOKEN_RPAREN) {
        list_push(param_nodes_list, parse_fun_param(parser));
        if (parser->current_token.type == TOKEN_COMMA) { // for handling case where this is the last parameters (and there is no comma after)
            parser_eat_assumes(parser, TOKEN_COMMA);
        }
    }
    parser_eat_assumes(parser, TOKEN_RPAREN);

    parser_eat_assumes(parser, TOKEN_COLON);
    Token token_type = parser_eat_assumes(parser, TOKEN_TYPE);
    Char* type = strdup(token_type.value);

    // handling codeblock
    CodeblockNode* codeblock = parse_code_block(parser);

    return fun_def_node_new(name, type, param_nodes_list, codeblock);
}

static StmtNode* parse_start_def(Parser* parser)
{
    // eat _start keyword
    parser_eat_assumes(parser, TOKEN_ID);

    // handling codeblock
    CodeblockNode* codeblock = parse_code_block(parser);

    return start_node_new(codeblock);
}

StmtNode* instr_to_stmt_node(InstrNode* instr_node)
{
    StmtNode* stmt_node = (StmtNode*)malloc(sizeof(StmtNode));
    if (!stmt_node) {
        PANIC("failed to allocate memory");
    }
    stmt_node->type = NODE_INSTR;
    stmt_node->instr_node = *instr_node;
    free(instr_node);
    return stmt_node;
}

static StmtNode* parse_stmt(Parser* parser)
{
    if (parser->current_token.type == TOKEN_FUN) {
        StmtNode* fun_def_node = parse_fun_def(parser);
        return fun_def_node;
    }
    if (parser->current_token.type == TOKEN_ID && strcmp(parser->current_token.value, "_start") == 0) {
        StmtNode* fun_def_node = parse_start_def(parser);
        return fun_def_node;
    }
    else {
        StmtNode* instr_node = instr_to_stmt_node(parse_instr(parser));
        return instr_node;
    }
}

List* parse(const List* token_list)
{
    printf("Parsing...\n");

    Parser* parser = parser_new(token_list);

    List* statement_list = list_new(sizeof(StmtNode));
    while (parser->index < parser->token_list_size) {
        if (parser->current_token.type == TOKEN_EOF) {
            break;
        }
        StmtNode* stmt = parse_stmt(parser);
        list_push(statement_list, stmt);
    }

    parser_free(parser);
    return statement_list;
}
