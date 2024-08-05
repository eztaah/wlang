#include "parser.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    const List* token_list;
    I32 token_list_size;
    I32 index;
    Token current_token;
} Parser;

static Parser* instanciate_parser(const List* token_list)
{
    Parser* parser = calloc(1, sizeof(Parser));
    parser->token_list = token_list;
    parser->token_list_size = token_list->size;
    parser->index = 0;
    parser->current_token = *((Token*)token_list->items[parser->index]);

    return parser;
}

static Token parser_eat(Parser* parser)
{
    Token current_token = parser->current_token;
    parser->index++;
    if (parser->index < parser->token_list_size) {
        parser->current_token = *((Token*)parser->token_list->items[parser->index]);
    }
    return current_token;
}

static Token parser_eat_assumes(Parser* parser, TokenType expected_token_type)
{
    Token token = parser->current_token;
    if (token.type != expected_token_type) {
        printf("Error: expected token type %d but got %d\n", expected_token_type, token.type);
        exit(EXIT_FAILURE);
    }
    return parser_eat(parser);
}

static ExpressionNode* parse_term(Parser* parser)
{
    if (parser->current_token.type == TOKEN_INT) {
        Token token = parser_eat_assumes(parser, TOKEN_INT);
        return instanciate_number_node(token.value);
    }
    return NULL;
}

static ExpressionNode* parse_expr(Parser* parser) 
{
    ExpressionNode* left = parse_term(parser);
    while (parser->index < parser->token_list_size && 
           (parser->current_token.type == TOKEN_PLUS || parser->current_token.type == TOKEN_MINUS || parser->current_token.type == TOKEN_MUL || parser->current_token.type == TOKEN_DIV)) {
        Token op = parser_eat(parser);
        ExpressionNode* right = parse_term(parser);
        left = instanciate_binop_node(left, op.type, right);
    }
    return left;
}

static VarDeclNode* parse_var_decl(Parser* parser)
{
    // handling cst and var keywords
    parser_eat(parser);

    // handling name
    Token name_token = parser_eat_assumes(parser, TOKEN_ID);
    char* name = strdup(name_token.value);

    parser_eat_assumes(parser, TOKEN_COLON);

    // handling type
    Token type_token = parser_eat_assumes(parser, TOKEN_TYPE);
    char* type = strdup(type_token.value);

    // handling content
    parser_eat_assumes(parser, TOKEN_EQUAL);
    ExpressionNode* value = parse_expr(parser);

    return instanciate_var_decl_node(type, name, value);
}

static StatementNode* parse_stmt(Parser* parser) 
{
    if (parser->current_token.type == TOKEN_VAR || parser->current_token.type == TOKEN_CST) {
        VarDeclNode* var_decl = parse_var_decl(parser);
        StatementNode* stmt = malloc(sizeof(StatementNode));
        stmt->type = NODE_VAR_DECL;
        stmt->var_decl = *var_decl;
        return stmt;
    }
    // If no valid statement is found, consume the current token to advance the parser
    parser_eat(parser);
    return NULL;
}

List* parse(const List* token_list)
{
    Parser* parser = instanciate_parser(token_list);
    List* statement_list = init_list(sizeof(StatementNode));
    
    while (parser->index < parser->token_list_size) {
        StatementNode* stmt = parse_stmt(parser);
        if (stmt) {
            list_push(statement_list, stmt);
        }
    }
    return statement_list;
}
