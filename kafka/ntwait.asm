EXTERN g_NtWaitForSingleObjectSyscall : DWORD
PUBLIC NtWaitForSingleObject_syscall

.code
NtWaitForSingleObject_syscall PROC
    mov r10, rcx
    mov eax, DWORD PTR [g_NtWaitForSingleObjectSyscall]
    syscall
    ret
NtWaitForSingleObject_syscall ENDP
END
