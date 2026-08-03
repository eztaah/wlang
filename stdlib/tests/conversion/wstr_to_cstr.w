glb <64> main() {
    !wstr <64> wstr[6] = "hello\n";
    !cstr& <64> cstr = wstr_to_cstr(wstr, 6);

    print_cstr(cstr, 6);
    ret 0; 
}

: expected output : hello\n
