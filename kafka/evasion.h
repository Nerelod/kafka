#pragma once
#include <windows.h>
void dircheck(const char* expectedName) {
    char path[MAX_PATH];
    char* lastSlash;
    char* parentName;

    // Get full path of current executable
    if (!GetModuleFileNameA(NULL, path, MAX_PATH)) {
        ExitProcess(1);
    }

    // Remove executable name
    lastSlash = strrchr(path, '\\');
    if (!lastSlash) {
        ExitProcess(1);
    }
    *lastSlash = '\0';

    // Find parent directory name
    parentName = strrchr(path, '\\');
    if (!parentName) {
        ExitProcess(1);
    }
    parentName++; // move past '\'

    // Compare
    if (_stricmp(parentName, expectedName) != 0) {
        ExitProcess(1);
    }
}