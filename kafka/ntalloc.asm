EXTERN g_NtAllocateVirtualMemorySyscall : DWORD
PUBLIC NtAllocateVirtualMemory_syscall

.code
NtAllocateVirtualMemory_syscall PROC
    mov r10, rcx
    mov eax, g_NtAllocateVirtualMemorySyscall
    syscall
    ret
NtAllocateVirtualMemory_syscall ENDP
END

