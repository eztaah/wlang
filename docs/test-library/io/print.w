export printstr
export printnb

# this function is a wrapper of the C printf function  
# 
fun printstr(cst message: &C8): Void
{
    !compiler_print(message1)
}

fun printnb(cst message1: &C8, cst number: I32, cst message2: &C8): Void
{
    !compiler_print_with_number(message1, number, message2)
}
