EXTERN g_NtDelayExecutionSyscall : DWORD
PUBLIC NtDelayExecution_syscall

.code
NtDelayExecution_syscall PROC
    mov r10, rcx
    mov eax, DWORD PTR [g_NtDelayExecutionSyscall]
    syscall
    ret
NtDelayExecution_syscall ENDP
END