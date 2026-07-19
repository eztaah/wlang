# wlang

The goal is to create a simple high-level programming language.  
The compiler is written in C and translates your high-level code into x86_64 GNU assembly.

<br>

## Language Syntax

> A Visual Studio Code extension for syntax highlighting is available:  
[https://marketplace.visualstudio.com/items?itemName=eztaah.w](https://marketplace.visualstudio.com/items?itemName=eztaah.w)

<br>

Below is a program that adds two numbers and prints the sum to the terminal:  
<img src="./docs/language-preview.png" alt="image" width="575">

Key Points:
- The `return` keyword is mandatory, even if the function returns nothing.
- Functions prefixed with `@` are directly translated to assembly during compilation and do not have "definitions".
- The only existing type is `I64` (a signed 64-bit integer). Keep in mind that you can store ASCII codes and even memory addresses in it. Therefore, it's possible to manipulate characters and addresses.
- Instructions cannot be written outside of functions.

<br>

## How to use the compiler

To compile your code, use the `wlangc` program (see below for building the compiler).

```
$ wlangc --help                                    
Usage:
    wlangc <file.w> [options]     compile a source file with optional flags.
    wlangc --version              display compiler version and exit.
    wlangc --help                 display this help message and exit.

Options:
    -v, --verbose                   output additional information
    -d, --dev-mode                  activate dev mode (add comments in the asm code, ...)
    -e, --to-executable             assemble and link the asmed assembly code into an executable
                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.
                                        - the generated executable will only run on x86_64 architecture and requires a Linux system.
``` 

<br>

## Building the compiler from source

Navigate to the project directory and execute this command:
```shell
cd compiler
make release
``` 

A binary named `wlangc` will be generated in your current directory.