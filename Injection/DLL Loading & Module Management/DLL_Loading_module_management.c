#include <windows.h>
#include <stdio.h>
#include <psapi.h>

// Function pointer type for LdrLoadDll (native API)
typedef NTSTATUS (NTAPI *pLdrLoadDll)(
    PWCHAR PathToFile,
    ULONG Flags,
    PVOID ModuleFileName,  // PUNICODE_STRING
    PHANDLE ModuleHandle
);

// UNICODE_STRING structure for LdrLoadDll
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// Helper function to initialize UNICODE_STRING
void InitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) {
    if (SourceString) {
        DestinationString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR));
        DestinationString->MaximumLength = DestinationString->Length + sizeof(WCHAR);
    } else {
        DestinationString->Length = 0;
        DestinationString->MaximumLength = 0;
    }
    DestinationString->Buffer = (PWCHAR)SourceString;
}

// Demonstrate LoadLibraryA - Standard DLL loading
void DemoLoadLibraryA() {
    printf("\nLoadLibraryA - Standard DLL Loading\n");
    
    
    const char* dllName = "user32.dll";
    printf("Attempting to load: %s\n", dllName);
    
    HMODULE hModule = LoadLibraryA(dllName);
    
    if (hModule != NULL) {
        printf("[+] DLL loaded successfully!\n");
        printf("    Module Handle: 0x%p\n", (void*)hModule);
        
        char modulePath[MAX_PATH];
        DWORD size = GetModuleFileNameA(hModule, modulePath, MAX_PATH);
        
        if (size > 0) {
            printf("    Module Path: %s\n", modulePath);
        }
        
        printf("    Base Address: 0x%p\n", (void*)hModule);
        
        FARPROC procAddr = GetProcAddress(hModule, "MessageBoxA");
        if (procAddr) {
            printf("    MessageBoxA Address: 0x%p\n", (void*)procAddr);
        }
        
        printf("[!] Important: Always call FreeLibrary() to unload\n");
        FreeLibrary(hModule);
        printf("[+] DLL unloaded successfully\n");
        
    } else {
        printf("[-] Failed to load DLL\n");
        printf("    Error Code: %lu\n", GetLastError());
    }
}

// LoadLibraryExA - Extended DLL loading
void DemoLoadLibraryExA() {
    printf("\nLoadLibraryExA - Extended DLL Loading\n");

    
    const char* dllName = "kernel32.dll";
    
    printf("Available Flags:\n");
    printf("  DONT_RESOLVE_DLL_REFERENCES (0x01) - Don't resolve imports\n");
    printf("  LOAD_LIBRARY_AS_DATAFILE (0x02) - Load as data file\n");
    printf("  LOAD_WITH_ALTERED_SEARCH_PATH (0x08) - Altered search path\n");
    printf("  LOAD_IGNORE_CODE_AUTHZ_LEVEL (0x10) - Ignore authorization\n");
    printf("  LOAD_LIBRARY_AS_IMAGE_RESOURCE (0x20) - Load as image resource\n\n");
    
    // Example 1: Normal loading
    printf("Example 1: Normal loading (no flags)\n");
    HMODULE hModule1 = LoadLibraryExA(dllName, NULL, 0);
    if (hModule1) {
        printf("[+] Loaded with default behavior\n");
        printf("    Handle: 0x%p\n", (void*)hModule1);
        FreeLibrary(hModule1);
    }
    printf("\n");
    
    // Example 2: Load as data file
    printf("Example 2: Load as data file (LOAD_LIBRARY_AS_DATAFILE)\n");
    HMODULE hModule2 = LoadLibraryExA(dllName, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (hModule2) {
        printf("[+] Loaded as data file (no code execution)\n");
        printf("    Handle: 0x%p\n", (void*)hModule2);
        printf("[*] This is safe for resource inspection only\n");
        FreeLibrary(hModule2);
    }
    printf("\n");
    
    // Example 3: Don't resolve DLL references
    printf("Example 3: Don't resolve imports (DONT_RESOLVE_DLL_REFERENCES)\n");
    HMODULE hModule3 = LoadLibraryExA(dllName, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hModule3) {
        printf("[+] Loaded without resolving imports\n");
        printf("    Handle: 0x%p\n", (void*)hModule3);
        printf("[!] Import table not resolved - can't call functions\n");
        FreeLibrary(hModule3);
    }
}

// LdrLoadDll - Native NT loader
void DemoLdrLoadDll() {
    printf("\nLdrLoadDll - Native NT DLL Loader\n");
    
    printf("[*] LdrLoadDll is the native NT API for loading DLLs\n");
    printf("[*] Used internally by LoadLibrary functions\n");
    printf("[*] Provides lower-level control over loading process\n\n");
    
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[-] Failed to get ntdll.dll handle\n");
        return;
    }
    
    pLdrLoadDll LdrLoadDll = (pLdrLoadDll)GetProcAddress(hNtdll, "LdrLoadDll");
    if (!LdrLoadDll) {
        printf("[-] Failed to get LdrLoadDll address\n");
        return;
    }
    
    printf("[+] Located LdrLoadDll function\n");
    printf("    Address: 0x%p\n\n", (void*)LdrLoadDll);
    
    WCHAR dllNameW[] = L"advapi32.dll";
    UNICODE_STRING usDllName;
    InitUnicodeString(&usDllName, dllNameW);
    
    printf("Loading DLL via LdrLoadDll: %ls\n", dllNameW);
    printf("    Length: %u bytes\n", usDllName.Length);
    printf("    MaxLength: %u bytes\n\n", usDllName.MaximumLength);
    
    HANDLE hModule = NULL;
    NTSTATUS status = LdrLoadDll(
        NULL,
        0,
        &usDllName,
        &hModule
    );
    
    if (status == 0) {
        printf("[+] DLL loaded via native API!\n");
        printf("    Module Handle: 0x%p\n", (void*)hModule);
        printf("    NTSTATUS: 0x%08X (STATUS_SUCCESS)\n", status);
        
        char modulePath[MAX_PATH];
        if (GetModuleFileNameA((HMODULE)hModule, modulePath, MAX_PATH)) {
            printf("    Full Path: %s\n", modulePath);
        }
        
        printf("[*] Note: LdrLoadDll uses native NTSTATUS return codes\n");
        FreeLibrary((HMODULE)hModule);
        
    } else {
        printf("[-] LdrLoadDll failed\n");
        printf("    NTSTATUS: 0x%08X\n", status);
    }
}

// GetModuleHandleA - Module handle retrieval
void DemoGetModuleHandleA() {
    printf("\nGetModuleHandleA - Module Handle Retrieval\n");
    
    printf("[*] GetModuleHandleA retrieves handle to already loaded modules\n");
    printf("[*] Does NOT load the module - only queries existing modules\n");
    printf("[*] Returns NULL if module is not loaded\n\n");
    
    const char* systemModules[] = {
        NULL,
        "kernel32.dll",
        "ntdll.dll",
        "user32.dll",
        "advapi32.dll",
        "ws2_32.dll",
        "nonexistent.dll"
    };
    
    printf("Checking loaded modules in current process:\n\n");
    
    for (int i = 0; i < sizeof(systemModules) / sizeof(systemModules[0]); i++) {
        const char* moduleName = systemModules[i];
        
        if (moduleName == NULL) {
            printf("Checking: [Current Executable]\n");
        } else {
            printf("Checking: %s\n", moduleName);
        }
        
        HMODULE hModule = GetModuleHandleA(moduleName);
        
        if (hModule != NULL) {
            printf("[+] Module is loaded\n");
            printf("    Handle: 0x%p\n", (void*)hModule);
            
            char path[MAX_PATH];
            if (GetModuleFileNameA(hModule, path, MAX_PATH)) {
                printf("    Path: %s\n", path);
            }
            
            MODULEINFO modInfo;
            if (GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
                printf("    Size: %lu bytes (0x%lX)\n", modInfo.SizeOfImage, modInfo.SizeOfImage);
                printf("    Entry Point: 0x%p\n", modInfo.EntryPoint);
            }
            
        } else {
            printf("[!] Module not loaded\n");
            printf("    Error: %lu\n", GetLastError());
        }
        printf("\n");
    }
    
    printf("[*] Key Difference: GetModuleHandle vs LoadLibrary\n");
    printf("    GetModuleHandle: Query only, no ref count change\n");
    printf("    LoadLibrary: Loads + increments ref count\n");
}

// Enumerate all loaded modules
void EnumerateLoadedModules() {
    printf("\nEnumerate All Loaded Modules\n");
    
    printf("[*] Using EnumProcessModules to list all loaded DLLs\n\n");
    
    HANDLE hProcess = GetCurrentProcess();
    HMODULE hModules[1024];
    DWORD cbNeeded;
    
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        DWORD moduleCount = cbNeeded / sizeof(HMODULE);
        printf("[+] Found %lu loaded modules:\n\n", moduleCount);
        
        for (DWORD i = 0; i < moduleCount && i < 10; i++) {
            char moduleName[MAX_PATH];
            char modulePath[MAX_PATH];
            
            if (GetModuleFileNameA(hModules[i], modulePath, MAX_PATH)) {
                if (GetModuleBaseNameA(hProcess, hModules[i], moduleName, MAX_PATH)) {
                    printf("  [%lu] %s\n", i + 1, moduleName);
                    printf("      Base: 0x%p\n", (void*)hModules[i]);
                    printf("      Path: %s\n\n", modulePath);
                }
            }
        }
        
        if (moduleCount > 10) {
            printf("  ... and %lu more modules\n", moduleCount - 10);
        }
    } else {
        printf("[-] Failed to enumerate modules\n");
    }
}

int main() 
{
    printf("\nDLL Loading & Module Management Demo\n");
   
    DemoLoadLibraryA();
    DemoLoadLibraryExA();
    DemoLdrLoadDll();
    DemoGetModuleHandleA();
    EnumerateLoadedModules();
    
    printf("[+] All demonstrations completed successfully!\n");
    printf("[*] Press Enter to exit...\n");
    getchar();
    
    return 0;
}