glb !void _print_digit(!digit <64> num)
{
    if (num > 9) {
        @exit(1);
    }

    !ascii <64> asciicode = num + 48;
    %sysc(1, 1, &asciicode, 1, 0, 0, 0);
}