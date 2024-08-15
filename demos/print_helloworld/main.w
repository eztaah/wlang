! def void& @malloc(int size)  from libc
! def void  @free(void& addr)  from libc

!int main()
{
    !int str_length = 14;

    : allocate memory for the string
    !ascii& str_addr = @malloc(str_length);

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
    ^(str_addr + 104) = 0;     : '\0'

    : print the string
    %sysc(1, 1, str_addr, str_length*8, 0, 0, 0);

    @free(str_addr);

    ret 0;
}
