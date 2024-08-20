: wrapper for the standard c library's malloc
!void* _malloc_libc(!i64 size)
{
    ret @malloc(size);
}

: wrapper for mmap syscall
!void* _malloc_sysc(!i64 size)
{
    !i64 addr = 0;                 : NULL, let the kernel choose the address
    !i64 length = size;            : the size of the memory to allocate
    !i64 prot = 0x3;               : read and write permissions (PROT_READ | PROT_WRITE)
    !i64 flags = 0x22;             : MAP_PRIVATE | MAP_ANONYMOUS
    !i64 fd = -1;                  : no file descriptor, because we're using MAP_ANONYMOUS
    !i64 offset = 0;               : offset is zero since we're not mapping a file

    !void* allocated_memory = %sysc(9, addr, length, prot, flags, fd, offset, 0);

    ret allocated_memory;
}

glb !void* malloc(!i64 size)
{
#ifdef USE_LIBC
    ret _malloc_libc(size);
#else
    ret _malloc_sysc(size);
#endif
}
