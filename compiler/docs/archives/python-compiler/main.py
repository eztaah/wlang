#!/bin/python3

from lexer import lexer, pretty_print
from parser import parse, print_ast
from generator import generate_assembly


# ---------------------------------

code = """
a = 4 * 2 + 1;
b = 2; 

if (a == b) {
    r = 0;
    a = 7;
    if (a == 9) {
        b = 0;
    }
}
"""

print("\n--- CODE ---")
print(code)
print("--------------")


print("\n--- LEXER ---")
tokens = lexer(code)
pretty_print(tokens)
print("----------------")


print("\n--- PARSER ---")
ast = parse(tokens)
print_ast(ast)
print("----------------")


print("\n--- ASSEMBLY ---")
assembly_code, _ = generate_assembly(ast)
print("global _start\n")
print("_start:")
for instruction in assembly_code:
    print(instruction)
# end the programm
print("; exit syscall")
print("mov rax, 60")
print("mov rdi, 0")
print("syscall")
print("----------------------")



