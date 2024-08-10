#include <stdio.h> // snprintf(), NULL

#include "lib.h"
#include "token.h"

Str* print_tokenlist(const List* token_list)
{
    Str* output = str_new("[\n");

    for (I32 i = 0; i < token_list->size - 1; i++) {
        const Token* token = (Token*)token_list->items[i];
        if (token != NULL) {
            const Char* token_type_name = tokentype_to_string(token->type);

            Char temp[1024];
            snprintf(temp, sizeof(temp), "    {%s, \"%s\"},\n", token_type_name, token->value);
            str_cat(output, temp);

            if (token->type == TOKEN_END_INSTR) {
                str_cat(output, "\n");
            }
        }
    }
    str_cat(output, "]");

    return output;
}