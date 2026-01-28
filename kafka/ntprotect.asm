EXTERN g_NtProtectVirtualMemorySyscall : DWORD
PUBLIC NtProtectVirtualMemory_syscall

.code
NtProtectVirtualMemory_syscall PROC
    mov r10, rcx
    mov eax, g_NtProtectVirtualMemorySyscall
    syscall
    ret
NtAllocateVirtualMemory_syscall ENDP
END
