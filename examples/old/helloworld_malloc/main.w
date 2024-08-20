! include malloc(), free(), print_string() from std

!int main()
{
    !int <64> str_length = 15;

    : allocate memory for the string
    !ascii& <64> str_addr = @malloc(str_length*8);

    : define the string "Hello, World!"
    ^(str_addr + 0) = 72;      : 'H'
    ^(str_addr + 8) = 101;     : 'e'
    ^(str_addr + 16) = 108;    : 'l'
    ^(str_addr + 24) = 108;    : 'l'
    ^(str_addr + 32) = 111;    : 'o'
    ^(str_addr + 40) = 44;     : ','
    ^(str_addr + 48) = 32;     : ' '
    ^(str_addr + 56) = 87;     : 'W'
    ^(str_addr + 64) = 111;    : 'o'
    ^(str_addr + 72) = 114;    : 'r'
    ^(str_addr + 80) = 108;    : 'l'
    ^(str_addr + 88) = 100;    : 'd'
    ^(str_addr + 96) = 33;     : '!'
    ^(str_addr + 104) = 10;    : '\n'
    ^(str_addr + 112) = 0;     : '\0'

    : print the string
    %sysc(1, 1, str_addr, str_length*8, 0, 0, 0);

    @free(str_addr);

    ret 0;
}