#include <windows.h>
#include <winternl.h>
#include <stdio.h>

DWORD g_NtAllocateVirtualMemorySyscall = 0;
DWORD g_NtProtectVirtualMemorySyscall = 0;

DWORD GetSyscallFromDisk10(const char* name){
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

DWORD GetSyscallFromDisk11(const char* name) {
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

void InitSyscalls(){
    g_NtAllocateVirtualMemorySyscall = GetSyscallFromDisk10("NtAllocateVirtualMemory");
    g_NtProtectVirtualMemorySyscall = GetSyscallFromDisk10("NtProtectVirtualMemory");
}

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

void* h_ntallocatevirtualmemory(SIZE_T size, ULONG protect){
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
    
    NTSTATUS status = NtProtectVirtualMemory_syscall(
        GetCurrentProcess(),
        (PVOID*)&buf,
        &size,
        newprotect,
        oldprotect
    );
}

int main(void){
    InitSyscalls();
    char* buf = (char*)h_ntallocatevirtualmemory(0x1000, PAGE_READWRITE);
    printf("buf=%p\n", buf);
    if (!buf) {
        return -1;
    }

    strcpy_s(buf, 0x1000, "Hello from syscall-backed allocator");
    puts(buf);
    fflush(stdout);
    
    PULONG oldprotect;
    h_ntprotectvirtualmemory(buf, 0x1000, PAGE_EXECUTE_READWRITE, &oldprotect); 
    MessageBoxA(NULL, "this is a message", "Debug", MB_OK);

    return 0;
}
