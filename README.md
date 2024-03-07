```
- the exension name is cp
- check the vscode extension for syntax highlighting
```


## Syntax 

```python
# this is a comment

"""
this is a multi-line comment

- negative number are not supported
- i64 is the only type supported
- for loops are not supported
"""

# declare variables
# all of these operators are supported and follow this order ( ./docs/operator-order.txt )
var a: i64 = 3 ^ 4 | 8 + 7 * 4 << 1 - 8 >> 2 & 4 // 2;

# modify variables
a = 0

# declare constants
cst b: i64 = (1 + 2) * 3; 

# print function
print(a);
print("b is equal to : ", b, "\n");

# input function
a = input();        # for exemple we enter 5

# if statement
if (a == 5) {
    print("wow, a is equal to 5\n");
}
else {
    print("...");
}

# while loop
while (a < b) {
    a = a + 1;
}
``` 

<br>

## How to use the compiler

Ensure you have `nasm` and `GNU ld` installed on your Linux system.
```
$ compiler --help                                    
Usage:
    compiler <file.cp> [options]    compile a source file with optional flags.
    compiler --version              display compiler version and exit.
    compiler --help                 display this help message and exit.

Options:
    -o, --output <executable>       Specify the location and name of the output executable (default: ./prog).
    -b, --build-dir <directory>     Specify the build directory for intermediate files. (default: ./build/)
    -d, --dev-mode                  activate dev mode
    -r, --runtime-error             throw runtime errors when an error occur
``` 

<br>

## Building from source

Navigate to the projet directory and execute this command :
```shell
make release
``` 

A binary named `compiler` will be generated in the project's root directory.