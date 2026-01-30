EXTERN g_NtCreateThreadExSyscall : DWORD
PUBLIC NtCreateThreadEx_syscall

.code
NtCreateThreadEx_syscall PROC
    mov r10, rcx
    mov eax, DWORD PTR [g_NtCreateThreadExSyscall]
    syscall
    ret
NtCreateThreadEx_syscall ENDP
END
