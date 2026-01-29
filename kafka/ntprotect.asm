EXTERN g_NtProtectVirtualMemorySyscall : DWORD
PUBLIC NtProtectVirtualMemory_syscall

.code
NtProtectVirtualMemory_syscall PROC
    mov r10, rcx
    mov eax, DWORD PTR [g_NtProtectVirtualMemorySyscall]
    syscall
    ret
NtProtectVirtualMemory_syscall ENDP
END
