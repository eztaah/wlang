%import strlen %from wlib

"""
Exemples : 
print("Bonjour a tous\n" + i32_to_c8(nombre) + "ca va ? " + mot)

"""
fun print(cst message: &c8): void
{
    cst len: i32 = strlen(message)

    # write syscall
    syscall(1, 1, message, len)
}


"""
Exemples : 
cst buffer: &c8     # allocate 1 byte memory
input(buffer)

"""
fun input(cst buffer: &c8): void
{
    syscall(0, 1, buffer, 1)       # the last one say that we will be reading one byte from stdin
}

