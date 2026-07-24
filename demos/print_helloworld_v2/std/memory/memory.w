glb !void& malloc(!int size)
{
    !int addr = 0;                 : NULL, let the kernel choose the address
    !int length = size;            : the size of the memory to allocate
    !int prot = 3;               : read and write permissions (PROT_READ | PROT_WRITE)
    !int flags = 34;             : MAP_PRIVATE | MAP_ANONYMOUS
    !int fd = -1;                  : no file descriptor, because we're using MAP_ANONYMOUS
    !int offset = 0;               : offset is zero since we're not mapping a file

    !void& allocated_memory = %sysc(9, addr, length, prot, flags, fd, offset);

    ret allocated_memory;
}

glb !void free(!void& addr, !int size)
{
    %sysc(11, addr, size, 0, 0, 0, 0);  : munmap syscall number is 11
}
