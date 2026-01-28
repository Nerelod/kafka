#include <windows.h>
#include <winternl.h>
#include <stdio.h>

DWORD g_NtAllocateVirtualMemorySyscall = 0;

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
}

extern NTSTATUS NtAllocateVirtualMemory_syscall(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

void* MyHeapAlloc(SIZE_T size)
{
    PVOID base = NULL;
    SIZE_T regionSize = size;

    NTSTATUS status = NtAllocateVirtualMemory_syscall(
        GetCurrentProcess(),
        &base,
        0,
        &regionSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (status != 0)
        return NULL;

    return base;
}

int main(void){
    InitSyscalls();
    char* buf = (char*)MyHeapAlloc(0x1000);
    if (!buf) {
        return -1;
    }

    strcpy_s(buf, 0x1000, "Hello from syscall-backed allocator");
    puts(buf);
    fflush(stdout);

    return 0;
}
