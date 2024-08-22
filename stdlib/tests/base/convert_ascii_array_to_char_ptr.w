glb !int <64> main()
{
    !ascii <64> original_msg[15] = "Hello, world\n\0"; 

    : Convert the ASCII array to a char pointer
    !ascii& <64> char_ptr = convert_ascii_array_to_char_ptr(original_msg, 15);

    : Use the C printf function to print the string
    @printf(char_ptr);

    : Free the memory allocated for the char pointer
    free(char_ptr, 15);

    ret 0;
}

: expected output : Hello, world!\n