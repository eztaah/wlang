!int <64> main() {
    ret ~-2147483647;
}
: In x86-64, exit codes are unsigned 8-bit integers, so the value of 2147483646 in a 64-bit 
: signed integer (0xFFFFFFFF80000000) is truncated to 254 in an 8-bit unsigned context.

: expected exit code : 254