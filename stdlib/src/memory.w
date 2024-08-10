: allocates a block of memory of the specified size
:
: this function allocates a block of memory of size `size` bytes. if the `USE_C_MEMORY_FUN`
: preprocessor flag is defined, the standard C `malloc` function is used. otherwise, the 
: function uses a system call to request memory from the operating system. the memory 
: is allocated with read and write permissions and is not backed by any file (anonymous mapping).
glb !void& malloc(<64> len)
{
#ifdef C_MEMORY
    !void& <64> allocated_memory = @malloc(len);
#else
    <64> addr = 0;                 : NULL, let the kernel choose the address
    : len                          : the size of the memory to allocate
    <64> prot = 0x3;               : read and write permissions (PROT_READ | PROT_WRITE)
    <64> flags = 0x22;             : MAP_PRIVATE | MAP_ANONYMOUS, memory not backed by a file
    <64> fd = -1;                  : no file descriptor, because we're using MAP_ANONYMOUS
    <64> offset = 0;               : offset is zero since we're not mapping a file

    : use the mmap system call to allocate memory
    !void& <64> allocated_memory = %sysc(9, addr, len, prot, flags, fd, offset);
#endif
    ret allocated_memory;
}

: frees a previously allocated block of memory
:
: this function frees a block of memory that was previously allocated with `malloc`.
: if the `USE_C_MEMORY_FUN` preprocessor flag is defined, the standard C `free` function is used.
: otherwise, the function uses a system call to unmap the memory region, effectively freeing it.
glb free(!void& <64> addr, <64> len)
{
#ifdef C_MEMORY
    @free(addr);
#else
    %sysc(11, addr, len, 0, 0, 0, 0)
#endif
}
