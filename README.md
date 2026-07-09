# wlang

The compiler transform your high level code (written with the syntax specified above) to x86_64 GNU assembly.

A Visual Studio Code extension is available for syntax highlighting :    
[https://marketplace.visualstudio.com/items?itemName=eztaah.w](https://marketplace.visualstudio.com/items?itemName=eztaah.w)

<br>

## Language syntax 

```python
# ---- VARIABLES ----

# declare decimal variable
var a: i64 = 2

# declare binary variable
var b: i64 = 0b1011

# declare constants
cst c: i64 = -7

# modify variable
a = 3

# i64 is the only supported type
# you cannot initialize variables without assigning them a value.



# ---- BLOCKS ----

# if
if (a == 1) {
    # ...
}
else {
    # ...
}

# while loop
while (a < 12) {
    # ...
}

# elif is not supported 
# for loops are not supported



# ---- LIBRARIE ----

# you have access to 3 built in functions

# print
# -> print number, variable or string to the standard ouput, need to \n manually
print(1 + 1)
print("Hello")
print(a)
print("The value is ", a, "\n")

# input
# -> store the user input to a variable, only support integer
a = input()

# exit
# exit the program with 0, (you can't specify an exit code)
exit()



# ---- OPERATIONS ----

# basic
+    *
-    //
%    

# binary
&    |
^

# bool
==    !=
<=    >=
<     >
&&    ||

# all of these operators are supported and follow this order ( ./docs/operator-order.txt )
# you can use parentheses
``` 

<br>

## How to use the compiler

```
$ compiler --help                                    
Usage:
    compiler <file.cp> [options]    compile a source file with optional flags.
    compiler --version              display compiler version and exit.
    compiler --help                 display this help message and exit.

Options:
    -o, --output-dir <directory>    specify the output directory. (default: ./out/)
    -d, --dev-mode                  activate dev mode
    -r, --runtime-error             throw runtime errors when an error occurs (useful for debugging)

    -e, --to-executable             assemble and link the generated assembly code into an executable
                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.
                                        - the generated executable will only be able to run on x86_64 CPUs and in a Linux environment.
``` 

<br>

## Building from source

Navigate to the projet directory and execute this command :
```shell
cd compiler
make release
``` 

A binary named `compiler` will be generated in your current directory.