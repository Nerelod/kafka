#pragma once
#include <windows.h>
#include <winternl.h>

extern NTSTATUS NtDelayExecution_syscall(
    BOOLEAN Alertable,
    PLARGE_INTEGER DelayInterval
);

extern NTSTATUS NtAllocateVirtualMemory_syscall(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

extern NTSTATUS NtProtectVirtualMemory_syscall(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PSIZE_T RegionSize,
    ULONG NewProtect,
    PULONG OldProtect
);

extern NTSTATUS NtCreateThreadEx_syscall(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    HANDLE ProcessHandle,
    PVOID StartRoutine,
    PVOID Argument,
    ULONG CreateFlags,
    SIZE_T ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximumStackSize,
    PVOID AttributeList
);

void h_ntdelayexecution(BOOLEAN alertable, PLARGE_INTEGER delayinterval) {
    NTSTATUS status = NtDelayExecution_syscall(
        alertable,
        delayinterval
    );
}

void* h_ntallocatevirtualmemory(SIZE_T size, ULONG protect) {
    PVOID base = NULL;
    SIZE_T regionSize = size;

    NTSTATUS status = NtAllocateVirtualMemory_syscall(
        GetCurrentProcess(),
        &base,
        0,
        &regionSize,
        MEM_COMMIT | MEM_RESERVE,
        protect
    );

    if (status != 0)
        return NULL;

    return base;
}

void h_ntprotectvirtualmemory(void* buf, SIZE_T size, ULONG newprotect, PULONG oldprotect) {
    PVOID base = buf;
    SIZE_T regionSize = size;
    NTSTATUS status = NtProtectVirtualMemory_syscall(
        GetCurrentProcess(),
        &base,
        &regionSize,
        newprotect,
        oldprotect
    );
}

void h_ntcreatethreadex(PHANDLE threadhandle, PVOID startroutine, PVOID argument) {
    NTSTATUS status = NtCreateThreadEx_syscall(
        threadhandle,
        THREAD_ALL_ACCESS,
        NULL,
        GetCurrentProcess(),
        startroutine,
        argument,
        FALSE,
        0,
        0,
        0,
        NULL
    );
}