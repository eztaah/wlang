# wlang compiler

This document contains information relevant to the wlang compiler.

<br>

- No short-circuit evaluation with `||` and `&&` operators; all expressions are fully evaluated.

- The language does not support floating-point types. Introducing a float type would necessitate assigning a type to every variable, as the compiler would need to determine whether to use `add` or `addss` for arithmetic operations. 

-  wlangc functions as both a compiler driver and a compiler, without a strict separation between the two.

<br>

```
Compilation phases :  

- Preprocessing                        |
- Lexing                 |             |
- Parsing                | compiler    | compiler driver
- Semantic analysis      |             |
- Assembly generation    |             |
- Assembling                           |
- Linking                              |
```

<br>

> The "types" in C exist primarily to prevent developers from making mistakes. Casting a signed integer to an unsigned integer does not change the content of the variable; it only changes the label on the variable, indicating that the value should be interpreted as unsigned. If this value is passed to a function expecting a signed type, an error will occur. However, **this does not alter the underlying assembly code**.

<br>

### Abstract Syntax Tree (AST)

```
program = Program(function_definitions*)

function_definitions = (identifier name, parameter*, block body)

parameter = Parameter(int size, identifier name)

block = Block(statement*)

statement = VarDeclaration(int size, identifier name, expr)

          | ArrayDeclaration(identifier name, int size, expr*)

          | Assignement(identifier name, expr)

          | Return(expr) 

          | If(expr condition, block true_block, block false_block)

          | Loop(block)

          | Break

          | Expression(exp)

expr = Constant(int)

    | Var(identifier)

    | VarAddr(identifier)

    | Unary(unary_operator, expr)

    | Binary(binary_operator, expr, expr)

    | FunctionCall(identifier name, expr* args)   

unary_operator = Negate | Complement | Not | Dereference

binary_operator = Add | Subtract | Multiply | Divide | Remainder 

                | And | Or

                | Equal | NotEqual | LessThan | LessOrEqual | GreaterThan | GreaterOrEqual

                | BinAnd | BinOr | RightShift | LeftShift | Xor |
```

<br>

### Operator Precedence

```
1. Parentheses Operations `()` have the highest priority and can be used to change the default order of evaluation.

2. Unary Operations such as binary NOT `~`, sign change `-`, and other unary operators.

3. Multiplication/Division Operations: Multiplication `*`, division `/`, and modulo `%`.

4. Addition and Subtraction Operations: Addition `+` and subtraction `-`.

5. Shift Operations: Left shift `<<` and right shift `>>`.

6. Binary AND Operation `&` comes after shift operations.

7. Binary XOR Operation `^` follows binary AND.

8. Binary OR Operation `|` comes after binary XOR, giving it a lower priority than binary AND and XOR.
```