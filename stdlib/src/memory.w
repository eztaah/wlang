glb !void& malloc(!int <64> size)
{
#ifdef USE_C_MEMORY_FUN
    !void <64> allocated_memory = @malloc(size);
#else
    !int <64> addr = 0;                 : NULL, let the kernel choose the address
    !int <64> length = size;            : the size of the memory to allocate
    !int <64> prot = 0x3;               : read and write permissions (PROT_READ | PROT_WRITE)
    !int <64> flags = 0x22;             : MAP_PRIVATE | MAP_ANONYMOUS
    !int <64> fd = -1;                  : no file descriptor, because we're using MAP_ANONYMOUS
    !int <64> offset = 0;               : offset is zero since we're not mapping a file

    !void& <64> allocated_memory = %sysc(9, addr, length, prot, flags, fd, offset);
#endif
    ret allocated_memory;
}

glb !void free(!void& <64> addr, !int <64> size)
{
#ifdef USE_C_MEMORY_FUN
    @free(addr);
#else
    !int <64> syscall_number = 11;      : SYS_munmap
    !int <64> memory_addr = addr;       : address of the memory to be freed
    !int <64> length = size;            : size of the memory region to unmap

    %sysc(syscall_number, memory_addr, length, 0, 0, 0, 0);
#endif
}
