# wlang compiler

This document provides a comprehensive overview of the wlang compiler, which transforms `.w` source files into x86_64 GNU assembly code.

<br>

## source code syntax and coding style

The compiler is written in C, you can see the use of custom types and structures :

- rename primitive types
    ```c
    typedef unsigned int U32;
    typedef size_t UX;
    typedef int I32;
    typedef ssize_t IX;
    typedef float F32;
    typedef char Char;
    typedef void Void;
    typedef enum { 
        FALSE = 0, 
        TRUE = 1 
    } Bool;
    ```

- new `Str` type
    ```c
    Str* my_str = str_new("Hello");
    str_cat(my_str, ", World!");
    str_free(my_str);
    ```

- new `List` type
    ```c
    List* my_list = list_new(sizeof(Char*));
    list_push(my_list, str_new("Hello"));
    Char* item = list_get(my_list, 0);
    list_free(my_list);
    ```

- new `Dict` type
    ```c
    Dict* my_dict = dict_new();
    dict_put(my_dict, "key", 42);
    I32 value = dict_get(my_dict, "key");
    dict_free(my_dict);
    ```

<br>
<br>

## how the compiler works

The wlang compiler processes source files in three major steps: 
- lexing (<explication tres rapide du role du lexer, 1 ligne>)
- parsing (<meme chose>)
- assembly generation (<meme chose>)

### lexing

Given the following WLang code:

```wlang
fun add(cst a: I32, cst b: I32): I32
{
    cst sum: I32 = a + b;
    return sum;
}
```

The lexer would output a token stream like this:

```plaintext
[FUN, ADD, CST, A, COLON, I32, COMMA, CST, B, COLON, I32, RPAREN, LBRACE, CST, SUM, COLON, I32, EQUAL, A, PLUS, B, SEMICOLON, RETURN, SUM, SEMICOLON, RBRACE]
```

<br>
<br>

### parsing

Starting with the token stream from the lexer:

```plaintext
FUN, ADD, CST, A, COLON, I32, COMMA, CST, B, COLON, I32, RPAREN, LBRACE,
CST, SUM, COLON, I32, EQUAL, A, PLUS, B, SEMICOLON,
RETURN, SUM, SEMICOLON,
RBRACE
```

The parser generate a syntax tree like this:

```plaintext
FunctionDefinition
├─ Name: "add"
├─ ReturnType: "I32"
├─ Parameters:
│  ├─ Param: "a", Type: "I32", Mutability: "const"
│  └─ Param: "b", Type: "I32", Mutability: "const"
├─ Body:
│  ├─ VariableDefinition: "sum", Type: "I32", Value: "a + b"
│  └─ Return: "sum"
```

<br>
<br>

### assembly Generation

Given the syntax tree from the parser:

```plaintext
FunctionDefinition
├─ Name: "add"
├─ ReturnType: "I32"
├─ Parameters:
│  ├─ Param: "a", Type: "I32", Mutability: "const"
│  └─ Param: "b", Type: "I32", Mutability: "const"
├─ Body:
│  ├─ VariableDefinition: "sum", Type: "I32", Value: "a + b"
│  └─ Return: "sum"
```

The assembly generator produce the following code:

```assembly
.global _add
_add:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $16, %rsp

    movq    %rdi, -8(%rbp)      # Move 'a' into the stack
    movq    %rsi, -16(%rbp)     # Move 'b' into the stack

    movq    -8(%rbp), %rax      # Load 'a' into %rax
    addq    -16(%rbp), %rax     # Add 'b' to %rax

    movq    %rax, -24(%rbp)     # Store the result in 'sum'

    movq    -24(%rbp), %rax     # Load 'sum' into %rax
    leave
    ret
```