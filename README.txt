c++ compiler


# TODO
- configurer ou install code formatter c++

- output l'ast en json
- ajouter commentaires  //, ou #, ou /*  */
- faire une analyse sémantique (verifier les const par exemple)
- faire un global.hpp
- ajout syntaxe
- implémentation des print et println, non-prioritairement, la fonction input
- faire les outputs de tt les phases dans des fichiers text (dans build)
      - build/output.lex
      - build/output.ast
      - build/output.cpp OU build/output.asm

V1:
println("test" + 123)
- concatenation + number, string
- type string

V2:
println("test", 123)   // arguments variadiques


class FunCall : public Node {
    std::string _name;
    std::vector<Node> parameters;
};






-----------------------------------------


# First approximative BNF
Meta characters are < > | ( ) ? * [ ] '

    <ROOT> ::= <statements>

    <statements> ::= <statement> ( <statement> )?

    <statement> ::= ( 
          <variable_declaration_statement>
        | <variable_assignment_statement>
        | <if_statement>
        | <while_statement>
      ) (;)*

    <expr> ::= 
        <number> 
      | <identifier> 
      | <string>
      | <expr> <binary_operator> <expr>
      | <unary_operator> <expr>
      | '(' <expr> ')'
 
    <identifier> ::= [a-zA-Z][a-zA-Z0-9]*

    <string> ::= 
        " .* " 
      | ' .* '
    // TODO: Add support for string escape \", \', etc

    <binary_operator> ::= '+' | '-' | '*' | '/' | '=='

    <unary_operator> ::= '++' | '--'
 
    <variable_declaration_statement> ::= <variable_keyword> <variable_name> (: <type>)? = <expr>

    <variable_assignment_statement> ::= <variable_name> = <expr>
      
    <if_statement> ::=
      if '(' <expr> ')' {
          <statements>
      } [ 
        else '(' <expr> ')' { 
          <statements> 
        }
     ]
     // TODO: Add support for "else if"

    <while_statement> ::= while '(' <expr> ')' { <statements> }

    <for_statement> ::= for '(' <variable_keyword> <identifier> in <expr> ')' { <statements> }

    <variable_keyword> ::= let | var

    <type> ::= 
        i8 
      | i16 
      | i32 
      | i64
      | i128
      | u8 
      | u16 
      | u32 
      | u64
      | u128
      | f32
      | f64
      | string

    <function_statement> ::= fun <identifier> '(' ( <identifier>: <type> )* ')' (: <type>)?  { <statements> }
    // TODO: fix this definition for arguments
