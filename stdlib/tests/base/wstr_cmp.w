glb <64> main() {
    !wstr& <64> str1[4] = "test";
    !wstr& <64> str2[4] = "test";
    !wstr& <64> str3[4] = "tese";
    !wstr& <64> str4[3] = "tes";

    if (wstr_cmp(str1, str2, 4, 4) != 1) {
        ret 1;  : Test failed
    }

    if (wstr_cmp(str1, str3, 4, 4) != 0) {
        ret 1;  : Test failed
    }

    if (wstr_cmp(str1, str4, 4, 3) != 0) {
        ret 1;  : Test failed
    }

    ret 0;  : All tests passed
}
: expected exit code : 0