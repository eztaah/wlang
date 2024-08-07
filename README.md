# wlang

The compiler transform your high level code (written with the syntax specified above) to x86_64 GNU assembly.

A Visual Studio Code extension is available for syntax highlighting :    
[https://marketplace.visualstudio.com/items?itemName=eztaah.w](https://marketplace.visualstudio.com/items?itemName=eztaah.w)

<br>

## Language syntax 

see `lang/SYNTAX.md` 

<br>

## How to use the compiler

```
$ wcompiler --help                                    
Usage:
    compiler <file.cp> [options]    compile a source file with optional flags.
    compiler --version              display compiler version and exit.
    compiler --help                 display this help message and exit.

Options:
    -o, --output-dir <directory>    specify the output directory. (default: ./out/)
    -d, --dev-mode                  activate dev mode

    -e, --to-executable             assemble and link the generated assembly code into an executable
                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.
                                        - the generated executable will only run on x86_64 architecture and requires a Linux system with the GNU C Library (glibc).
``` 

<br>

## Building from source

Navigate to the projet directory and execute this command :
```shell
cd compiler
make release
``` 

A binary named `wcompiler` will be generated in your current directory.