: prints the version of the libw library.
glb print_libw_version()
{
    !wstr& <64> version[11] = "libw 0.3.0\n";
    print_wstr(version, 11);
}

: exits the program with a given status code.
:
: this function serves as a low-level interface to the operating system's exit system call.
: the status code is passed as a 64-bit integer but will be interpreted as an unsigned 8-bit integer.
: 
: the function triggers a syscall (system call) to terminate the program with the given status.
glb exit(<64> status)
{
#ifdef C_EXIT
    @exit(status);
#else
    %sysc(60, status, 0, 0, 0, 0, 0);
#endif
}

: suspends execution for the specified duration in second and nanoseconds using the nanosleep system call
: 
: the function may fail and exit the program if there is an issue with memory allocation
_nanosleep(<64> seconds, <64> nanoseconds)
{
    !timespec_struct <64> timespec = malloc(16);
    ^(timespec + 0) = seconds;
    ^(timespec + 8) = nanoseconds;

    %sysc(35, timespec, 0, 0, 0, 0, 0);

    free(timespec);
}

: suspends execution for the specified duration in milliseconds
: 
: this function is a wrapper around the nanosleep function. it converts milliseconds into seconds and nanoseconds and calls nanosleep
glb sleep(<64> milliseconds)
{
#ifdef C_SLEEP
    <64> microseconds = milliseconds * 1000;
    @usleep(microseconds);
#else
    : convert milliseconds to seconds and nanoseconds and call _nanosleep
    <64> seconds = milliseconds / 1000;
    <64> nanoseconds = (milliseconds % 1000) * 1000000;
    _nanosleep(seconds, nanoseconds);
#endif
}
