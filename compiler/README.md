# wlang compiler

Ce document contient des informations pertinentes par rapport au compilateur 

This document contains internal documentation for compiler developpement

<br>


# Compiling phases 

wlangc est un compiler driver et un compilateur (pas de separation entre les 2 pour conveinance)

COMPILER DRIVER 
    COMPILER 
        - preprocessing
        - lexer 
        - parser
        - semantic analysis

    - assembling 
    - linking



# AST

program = Program(function_declaration*)

function_declaration = (identifier name, identifier* params, block? body)

block = Block(statement*)

statement = Return(exp)

          | Expression(exp)

          | Assignment(identifier, exp)

          | If(exp condition, block then, block? else)

          | Compound(block)

          | Break

          | Continue

          | Loop(block body)

          | Null

exp = Constant(int)

    | Var(identifier)

    | Unary(unary_operator, exp)

    | Binary(binary_operator, exp, exp)

    | FunctionCall(identifier, exp* args)

unary_operator = Negate

binary_operator = Add | Subtract | Multiply | Divide



# Operator order : 

1. Parenthesis Operations `()` have the highest priority and can be used to change the default order of evaluation
.
2. Unary Operations such as binary NOT `~`, sign change `-`, and other unary operators.

3. Multiplication/Division Operations: Multiplication `*`, division `/`, and modulo `%`.

4. Addition and Subtraction Operations: Addition `+` and subtraction `-`.

5. Shift Operations: Left shift `<<` and right shift `>>`.

6. Binary AND Operation `&` comes after shift operations.

7. Binary XOR Operation `^` comes after binary AND.

8. Binary OR Operation `|` comes after binary XOR, giving it a lower priority than binary AND and XOR.








