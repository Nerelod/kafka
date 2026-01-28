#include <windows.h>
#include <winternl.h>
#include <stdio.h>

DWORD g_NtAllocateVirtualMemorySyscall = 0;
DWORD g_NtProtectVirtualMemorySyscall = 0;

DWORD GetSyscallFromDisk(const char* name){
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

void InitSyscalls(){
    g_NtAllocateVirtualMemorySyscall = GetSyscallFromDisk("NtAllocateVirtualMemory");
    g_NtProtectVirtualMemorySyscall = GetSyscallFromDisk("NtProtectVirtualMemory");
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
    if (!buf) {
        return -1;
    }

    strcpy_s(buf, 0x1000, "Hello from syscall-backed allocator");
    puts(buf);
    fflush(stdout);
    
    PULONG oldprotect;
    void h_ntprotectvirtualmemory(buf, 0x1000, PAGE_EXECUTE_READ, oldprotect); 
    MessageBoxA(Null, "this is a message", "Debug", MB_OK);

    return 0;
}
