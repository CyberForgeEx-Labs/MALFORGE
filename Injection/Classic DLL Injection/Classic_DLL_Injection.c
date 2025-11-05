/*
 * Classic DLL Injection Demonstration
 * 
 * This program demonstrates various DLL injection techniques using
 * Windows APIs. It includes methods for injecting a DLL into a target
 * process using different approaches.
 * 
 * Compilation: cl.exe Classic_DLL_Injection.c /link kernel32.lib ntdll.lib
 * Or: gcc Classic_DLL_Injection.c -o Classic_DLL_Injection.exe -lkernel32 -lntdll
 */

#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

// NT API definitions for NtCreateThreadEx (utilizing ntdll)
typedef NTSTATUS(NTAPI* pNtCreateThreadEx)(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    PVOID ObjectAttributes,
    HANDLE ProcessHandle,
    PVOID lpStartAddress,
    PVOID lpParameter,
    ULONG CreateFlags,
    SIZE_T ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximumStackSize,
    PVOID pUnknown
);

// Function prototypes
DWORD GetProcessIdByName(const char* processName);
BOOL InjectDLL_CreateRemoteThread(DWORD pid, const char* dllPath);
BOOL InjectDLL_CreateRemoteThreadEx(DWORD pid, const char* dllPath);
BOOL InjectDLL_NtCreateThreadEx(DWORD pid, const char* dllPath);
void DemonstrateLoadLibrary();
void DemonstrateGetModuleHandle();
void DemonstrateGetProcAddress();
void DemonstrateThreadOperations();

// Main function
int main(int argc, char* argv[]) 
{
    printf("\n            Classic DLL Injection Techniques Demonstration            \n");

    // Demonstrate LoadLibraryA and LoadLibraryExA
    DemonstrateLoadLibrary();

    // Demonstrate GetModuleHandleA
    DemonstrateGetModuleHandle();

    // Demonstrate GetProcAddress
    DemonstrateGetProcAddress();

    // Demonstrate Thread Operations
    DemonstrateThreadOperations();

    // DLL Injection demonstrations (requires target process)
    printf("\n");
    printf("DLL Injection Methods");
    
    if (argc < 3) {
        printf("[!] For DLL injection demonstration, usage:\n");
        printf("    %s <target_process_name> <dll_path>\n\n", argv[0]);
        printf("    Example: %s notepad.exe C:\\\\temp\\\\mydll.dll\n\n", argv[0]);
        printf("[*] Continuing with API demonstrations only...\n\n");
    } else {
        const char* targetProcess = argv[1];
        const char* dllPath = argv[2];

        printf("[*] Target Process: %s\n", targetProcess);
        printf("[*] DLL Path: %s\n\n", dllPath);

        // Find target process
        DWORD pid = GetProcessIdByName(targetProcess);
        if (pid == 0) {
            printf("[-] Failed to find process: %s\n", targetProcess);
            return 1;
        }

        printf("[+] Found target process (PID: %lu)\n\n", pid);

        // Method 1: CreateRemoteThread
        printf("[*] Method 1: CreateRemoteThread\n");
        if (InjectDLL_CreateRemoteThread(pid, dllPath)) {
            printf("[+] DLL injection successful!\n\n");
        } else {
            printf("[-] DLL injection failed!\n\n");
        }

        // Method 2: CreateRemoteThreadEx
        printf("[*] Method 2: CreateRemoteThreadEx\n");
        if (InjectDLL_CreateRemoteThreadEx(pid, dllPath)) {
            printf("[+] DLL injection successful!\n\n");
        } else {
            printf("[-] DLL injection failed!\n\n");
        }

        // Method 3: NtCreateThreadEx
        printf("[*] Method 3: NtCreateThreadEx\n");
        if (InjectDLL_NtCreateThreadEx(pid, dllPath)) {
            printf("[+] DLL injection successful!\n\n");
        } else {
            printf("[-] DLL injection failed!\n\n");
        }
    }

    printf("\nDemonstration Complete\n");

    return 0;
}


// Get process ID by name
DWORD GetProcessIdByName(const char* processName) {
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //enumerate all the process.
    
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[-] CreateToolhelp32Snapshot failed (Error: %lu)\n", GetLastError());
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, processName) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return pid;
}

// DLL Injection using CreateRemoteThread
BOOL InjectDLL_CreateRemoteThread(DWORD pid, const char* dllPath) {
    BOOL success = FALSE;
    HANDLE hProcess = NULL;
    LPVOID pRemoteBuf = NULL;
    HANDLE hThread = NULL;

    printf("    [*] Opening target process...\n");
    
    // Open target process
    hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE,
        pid
    );

    if (hProcess == NULL) {
        printf("    [-] OpenProcess failed (Error: %lu)\n", GetLastError());
        return FALSE;
    }
    printf("    [+] Process opened (Handle: 0x%p)\n", hProcess);

    // Allocate memory in target process
    printf("    [*] Allocating memory in target process...\n");
    SIZE_T dllPathLen = strlen(dllPath) + 1;
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dllPathLen, 
                                 MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pRemoteBuf == NULL) {
        printf("    [-] VirtualAllocEx failed (Error: %lu)\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Memory allocated at: 0x%p (%zu bytes)\n", pRemoteBuf, dllPathLen);

    // Write DLL path to target process
    printf("    [*] Writing DLL path to target process...\n");
    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(hProcess, pRemoteBuf, dllPath, dllPathLen, &bytesWritten)) {
        printf("    [-] WriteProcessMemory failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Written %zu bytes to target process\n", bytesWritten);

    // Get LoadLibraryA address
    printf("    [*] Getting LoadLibraryA address...\n");
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
    
    if (pLoadLibrary == NULL) {
        printf("    [-] GetProcAddress failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] LoadLibraryA address: 0x%p\n", pLoadLibrary);

    // Create remote thread
    printf("    [*] Creating remote thread...\n");
    hThread = CreateRemoteThread(hProcess, NULL, 0, pLoadLibrary, pRemoteBuf, 0, NULL);

    if (hThread == NULL) {
        printf("    [-] CreateRemoteThread failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Remote thread created (Handle: 0x%p)\n", hThread);

    // Wait for thread completion
    printf("    [*] Waiting for remote thread to complete...\n");
    DWORD waitResult = WaitForSingleObject(hThread, 5000);
    
    if (waitResult == WAIT_OBJECT_0) {
        printf("    [+] Remote thread completed successfully\n");
        success = TRUE;
    } else if (waitResult == WAIT_TIMEOUT) {
        printf("    [!] Remote thread wait timeout\n");
        success = TRUE; // May still have succeeded
    } else {
        printf("    [-] WaitForSingleObject failed (Error: %lu)\n", GetLastError());
    }

    // Cleanup
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return success;
}

// DLL Injection using CreateRemoteThreadEx
BOOL InjectDLL_CreateRemoteThreadEx(DWORD pid, const char* dllPath) {
    BOOL success = FALSE;
    HANDLE hProcess = NULL;
    LPVOID pRemoteBuf = NULL;
    HANDLE hThread = NULL;

    printf("    [*] Opening target process...\n");
    
    hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE,
        pid
    );

    if (hProcess == NULL) {
        printf("    [-] OpenProcess failed (Error: %lu)\n", GetLastError());
        return FALSE;
    }
    printf("    [+] Process opened (Handle: 0x%p)\n", hProcess);

    SIZE_T dllPathLen = strlen(dllPath) + 1;
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dllPathLen, 
                                 MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pRemoteBuf == NULL) {
        printf("    [-] VirtualAllocEx failed (Error: %lu)\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Memory allocated at: 0x%p\n", pRemoteBuf);

    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(hProcess, pRemoteBuf, dllPath, dllPathLen, &bytesWritten)) {
        printf("    [-] WriteProcessMemory failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Written %zu bytes\n", bytesWritten);

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
    
    if (pLoadLibrary == NULL) {
        printf("    [-] GetProcAddress failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] LoadLibraryA address: 0x%p\n", pLoadLibrary);

    // Use CreateRemoteThreadEx with extended parameters
    printf("    [*] Creating remote thread with CreateRemoteThreadEx...\n");
    hThread = CreateRemoteThreadEx(
        hProcess,
        NULL,
        0,
        pLoadLibrary,
        pRemoteBuf,
        0,
        NULL,
        NULL
    );

    if (hThread == NULL) {
        printf("    [-] CreateRemoteThreadEx failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Remote thread created (Handle: 0x%p)\n", hThread);

    DWORD waitResult = WaitForSingleObject(hThread, 5000);
    
    if (waitResult == WAIT_OBJECT_0) {
        printf("    [+] Remote thread completed\n");
        success = TRUE;
    } else if (waitResult == WAIT_TIMEOUT) {
        printf("    [!] Thread wait timeout\n");
        success = TRUE;
    }

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return success;
}

// DLL Injection using NtCreateThreadEx
BOOL InjectDLL_NtCreateThreadEx(DWORD pid, const char* dllPath) {
    BOOL success = FALSE;
    HANDLE hProcess = NULL;
    LPVOID pRemoteBuf = NULL;
    HANDLE hThread = NULL;

    printf("    [*] Opening target process...\n");
    
    hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE,
        pid
    );

    if (hProcess == NULL) {
        printf("    [-] OpenProcess failed (Error: %lu)\n", GetLastError());
        return FALSE;
    }
    printf("    [+] Process opened (Handle: 0x%p)\n", hProcess);

    SIZE_T dllPathLen = strlen(dllPath) + 1;
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dllPathLen, 
                                 MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pRemoteBuf == NULL) {
        printf("    [-] VirtualAllocEx failed (Error: %lu)\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Memory allocated at: 0x%p\n", pRemoteBuf);

    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(hProcess, pRemoteBuf, dllPath, dllPathLen, &bytesWritten)) {
        printf("    [-] WriteProcessMemory failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Written %zu bytes\n", bytesWritten);

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    
    if (pLoadLibrary == NULL) {
        printf("    [-] GetProcAddress failed (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] LoadLibraryA address: 0x%p\n", pLoadLibrary);

    // Get NtCreateThreadEx from ntdll.dll
    printf("    [*] Getting NtCreateThreadEx address...\n");
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    pNtCreateThreadEx NtCreateThreadEx = (pNtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx");
    
    if (NtCreateThreadEx == NULL) {
        printf("    [-] Failed to get NtCreateThreadEx address (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] NtCreateThreadEx address: 0x%p\n", NtCreateThreadEx);

    // Create remote thread using NtCreateThreadEx
    printf("    [*] Creating remote thread with NtCreateThreadEx...\n");
    NTSTATUS status = NtCreateThreadEx(
        &hThread,
        THREAD_ALL_ACCESS,
        NULL,
        hProcess,
        pLoadLibrary,
        pRemoteBuf,
        0,
        0,
        0,
        0,
        NULL
    );

    if (status != 0 || hThread == NULL) {
        printf("    [-] NtCreateThreadEx failed (Status: 0x%08lX)\n", status);
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("    [+] Remote thread created (Handle: 0x%p)\n", hThread);

    DWORD waitResult = WaitForSingleObject(hThread, 5000);
    
    if (waitResult == WAIT_OBJECT_0) {
        printf("    [+] Remote thread completed\n");
        success = TRUE;
    } else if (waitResult == WAIT_TIMEOUT) {
        printf("    [!] Thread wait timeout\n");
        success = TRUE;
    }

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return success;
}

// Demonstrate LoadLibraryA and LoadLibraryExA
void DemonstrateLoadLibrary() {
    printf("LoadLibraryA - Loading DLL into Process\n");

    // Load a system DLL
    printf("[*] Loading kernel32.dll using LoadLibraryA...\n");
    HMODULE hKernel32 = LoadLibraryA("kernel32.dll");
    
    if (hKernel32 != NULL) {
        printf("[+] LoadLibraryA succeeded!\n");
        printf("    Module Handle: 0x%p\n\n", hKernel32);
        // Don't free kernel32.dll as it's always loaded
    } else {
        printf("[-] LoadLibraryA failed (Error: %lu)\n\n", GetLastError());
    }

    // Load with LoadLibraryExA
    printf("[*] Loading user32.dll using LoadLibraryExA...\n");
    HMODULE hUser32 = LoadLibraryExA("user32.dll", NULL, 0);
    
    if (hUser32 != NULL) {
        printf("[+] LoadLibraryExA succeeded!\n");
        printf("    Module Handle: 0x%p\n");
        printf("    Flags: 0 (standard loading)\n\n");
        // Don't free user32.dll as it may be in use
    } else {
        printf("[-] LoadLibraryExA failed (Error: %lu)\n\n", GetLastError());
    }
}



// Demonstrate GetModuleHandleA
void DemonstrateGetModuleHandle() {
    printf("GetModuleHandleA - Getting Module Handle\n");

    const char* modules[] = {
        "kernel32.dll",
        "ntdll.dll",
        "user32.dll"
    };

    for (int i = 0; i < 3; i++) {
        printf("[*] Getting handle for: %s\n", modules[i]);
        HMODULE hModule = GetModuleHandleA(modules[i]);
        
        if (hModule != NULL) {
            printf("[+] GetModuleHandleA succeeded!\n");
            printf("    Module Handle: 0x%p\n\n", hModule);
        } else {
            printf("[-] Module not loaded or not found\n\n");
        }
    }
}

// Demonstrate GetProcAddress
void DemonstrateGetProcAddress() {
    printf("GetProcAddress - Getting Function Address from Module\n");

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    
    if (hKernel32 == NULL) {
        printf("[-] Failed to get kernel32.dll handle\n\n");
        return;
    }

    const char* functions[] = {
        "LoadLibraryA",
        "GetProcAddress",
        "VirtualAllocEx",
        "WriteProcessMemory"
    };

    for (int i = 0; i < 4; i++) {
        printf("[*] Getting address of: %s\n", functions[i]);
        FARPROC pFunc = GetProcAddress(hKernel32, functions[i]);
        
        if (pFunc != NULL) {
            printf("[+] GetProcAddress succeeded!\n");
            printf("    Function Address: 0x%p\n\n", pFunc);
        } else {
            printf("[-] GetProcAddress failed (Error: %lu)\n\n", GetLastError());
        }
    }
}

// Demonstrate Thread Operations
void DemonstrateThreadOperations() {
    printf("Thread Operations - OpenThread and WaitForSingleObject\n");

    // Get current thread handle
    HANDLE hCurrentThread = GetCurrentThread();
    DWORD currentThreadId = GetCurrentThreadId();
    
    printf("[*] Current Thread ID: %lu\n", currentThreadId);
    printf("[*] Current Thread Handle (pseudo): 0x%p\n\n", hCurrentThread);

    // Open the current thread with specific access
    printf("[*] Opening current thread with OpenThread...\n");
    HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | SYNCHRONIZE, FALSE, currentThreadId);
    
    if (hThread != NULL) {
        printf("[+] OpenThread succeeded!\n");
        printf("    Thread Handle: 0x%p\n", hThread);
        printf("    Access: THREAD_QUERY_INFORMATION | SYNCHRONIZE\n\n");
        
        // Demonstrate WaitForSingleObject with immediate timeout
        printf("[*] Testing WaitForSingleObject (timeout=0)...\n");
        DWORD waitResult = WaitForSingleObject(hThread, 0);
        
        if (waitResult == WAIT_TIMEOUT) {
            printf("[+] Thread is running (WAIT_TIMEOUT as expected)\n\n");
        } else {
            printf("[+] Wait result: %lu\n\n", waitResult);
        }
        
        CloseHandle(hThread);
    } else {
        printf("[-] OpenThread failed (Error: %lu)\n\n", GetLastError());
    }
}