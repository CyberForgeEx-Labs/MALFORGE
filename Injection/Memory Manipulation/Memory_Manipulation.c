#include <windows.h>
#include <stdio.h>
#include <winternl.h>

// Native API function prototypes
typedef NTSTATUS(NTAPI* pNtAllocateVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

typedef NTSTATUS(NTAPI* pNtProtectVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PSIZE_T RegionSize,
    ULONG NewProtect,
    PULONG OldProtect
);

typedef NTSTATUS(NTAPI* pNtReadVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    SIZE_T BufferSize,
    PSIZE_T NumberOfBytesRead
);

typedef NTSTATUS(NTAPI* pNtWriteVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    SIZE_T BufferSize,
    PSIZE_T NumberOfBytesWritten
);

// Function prototypes for VirtualAlloc2 (Windows 10+)
typedef PVOID(WINAPI* pVirtualAlloc2)(
    HANDLE Process,
    PVOID BaseAddress,
    SIZE_T Size,
    ULONG AllocationType,
    ULONG PageProtection,
    MEM_EXTENDED_PARAMETER* ExtendedParameters,
    ULONG ParameterCount
);

// Helper function to print section headers
void PrintSectionHeader(const char* title) {
    printf("\n==============================================================\n");
    printf("%s\n", title);
    printf("================================================================\n\n");
}

// Helper function to print memory protection flags
void PrintProtectionFlags(DWORD protect) {
    printf("    Protection: ");
    if (protect & PAGE_EXECUTE_READWRITE) printf("PAGE_EXECUTE_READWRITE ");
    else if (protect & PAGE_EXECUTE_READ) printf("PAGE_EXECUTE_READ ");
    else if (protect & PAGE_READWRITE) printf("PAGE_READWRITE ");
    else if (protect & PAGE_READONLY) printf("PAGE_READONLY ");
    else if (protect & PAGE_EXECUTE) printf("PAGE_EXECUTE ");
    else if (protect & PAGE_NOACCESS) printf("PAGE_NOACCESS ");
    printf("\n");
}

// VirtualAlloc - Allocates memory in current process
void Demo_VirtualAlloc() {
    PrintSectionHeader("VirtualAlloc - Local Process Memory Allocation");

    printf("[*] Allocating 4096 bytes in current process...\n");
    
    LPVOID pMemory = VirtualAlloc(
        NULL,                   // Let system choose address
        4096,                   // Size in bytes
        MEM_COMMIT | MEM_RESERVE, // Commit and reserve
        PAGE_READWRITE          // Read/write access
    );

    if (pMemory == NULL) {
        printf("[-] VirtualAlloc failed! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Memory allocated successfully!\n");
    printf("    Base Address: 0x%p\n", pMemory);
    printf("    Size: 4096 bytes\n");
    PrintProtectionFlags(PAGE_READWRITE);

    // Write test data
    const char* testData = "VirtualAlloc test data";
    memcpy(pMemory, testData, strlen(testData) + 1);
    printf("\n[*] Written test data: \"%s\"\n", testData);
    printf("[*] Verification read: \"%s\"\n", (char*)pMemory);

    // Cleanup
    if (VirtualFree(pMemory, 0, MEM_RELEASE)) {
        printf("[+] Memory freed successfully.\n");
    }
}

// VirtualAllocEx - Allocates memory in remote process
void Demo_VirtualAllocEx() {
    PrintSectionHeader("VirtualAllocEx - Remote Process Memory Allocation");

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    printf("[*] Creating target process (notepad.exe)...\n");
    
    if (!CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED,
        NULL, NULL, &si, &pi)) {
        printf("[-] Failed to create process! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);
    printf("[*] Allocating 8192 bytes in remote process...\n");

    LPVOID pRemoteMemory = VirtualAllocEx(
        pi.hProcess,            // Target process handle
        NULL,                   // Let system choose address
        8192,                   // Size
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (pRemoteMemory == NULL) {
        printf("[-] VirtualAllocEx failed! Error: %lu\n", GetLastError());
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return;
    }

    printf("[+] Remote memory allocated successfully!\n");
    printf("    Base Address: 0x%p\n", pRemoteMemory);
    printf("    Size: 8192 bytes\n");
    printf("    Process: notepad.exe (PID: %lu)\n", pi.dwProcessId);
    PrintProtectionFlags(PAGE_READWRITE);

    // Cleanup
    VirtualFreeEx(pi.hProcess, pRemoteMemory, 0, MEM_RELEASE);
    printf("\n[*] Remote memory freed.\n");
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("[+] Target process terminated and cleaned up.\n");
}

// VirtualProtect - Changes memory protection
void Demo_VirtualProtect() {
    PrintSectionHeader("VirtualProtect - Modifying Memory Protection");

    printf("[*] Allocating memory with PAGE_READWRITE...\n");
    
    LPVOID pMemory = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pMemory == NULL) {
        printf("[-] VirtualAlloc failed! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Memory allocated at: 0x%p\n", pMemory);
    PrintProtectionFlags(PAGE_READWRITE);

    // Write test data
    strcpy((char*)pMemory, "Testing protection change");
    printf("\n[*] Written test data successfully.\n");

    // Change protection to PAGE_EXECUTE_READWRITE
    printf("[*] Changing protection to PAGE_EXECUTE_READWRITE...\n");
    
    DWORD oldProtect;
    if (!VirtualProtect(pMemory, 4096, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("[-] VirtualProtect failed! Error: %lu\n", GetLastError());
        VirtualFree(pMemory, 0, MEM_RELEASE);
        return;
    }

    printf("[+] Protection changed successfully!\n");
    printf("    Old Protection: 0x%08lX\n", oldProtect);
    PrintProtectionFlags(PAGE_EXECUTE_READWRITE);
    printf("\n[*] Memory now has execute permissions.\n");
    printf("[*] Use case: JIT compilation, dynamic code generation\n");

    // Cleanup
    VirtualFree(pMemory, 0, MEM_RELEASE);
    printf("\n[+] Memory freed.\n");
}

// VirtualProtectEx - Changes remote process memory protection
void Demo_VirtualProtectEx() {
    PrintSectionHeader("VirtualProtectEx - Remote Memory Protection Change");

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    printf("[*] Creating target process...\n");
    
    if (!CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED,
        NULL, NULL, &si, &pi)) {
        printf("[-] Failed to create process! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

    // Allocate remote memory
    LPVOID pRemote = VirtualAllocEx(pi.hProcess, NULL, 4096,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pRemote == NULL) {
        printf("[-] VirtualAllocEx failed! Error: %lu\n", GetLastError());
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return;
    }

    printf("[+] Remote memory allocated at: 0x%p\n", pRemote);
    PrintProtectionFlags(PAGE_READWRITE);

    // Change remote memory protection
    printf("\n[*] Changing remote memory protection to PAGE_EXECUTE_READ...\n");
    
    DWORD oldProtect;
    if (!VirtualProtectEx(pi.hProcess, pRemote, 4096, PAGE_EXECUTE_READ, &oldProtect)) {
        printf("[-] VirtualProtectEx failed! Error: %lu\n", GetLastError());
    } else {
        printf("[+] Remote protection changed successfully!\n");
        printf("    Old Protection: 0x%08lX\n", oldProtect);
        PrintProtectionFlags(PAGE_EXECUTE_READ);
    }

    // Cleanup
    VirtualFreeEx(pi.hProcess, pRemote, 0, MEM_RELEASE);
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("\n[+] Cleanup completed.\n");
}

// VirtualAllocExNuma - NUMA-aware memory allocation
void Demo_VirtualAllocExNuma() {
    PrintSectionHeader("VirtualAllocExNuma - NUMA-Aware Memory Allocation");

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    printf("[*] Creating target process...\n");
    
    if (!CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED,
        NULL, NULL, &si, &pi)) {
        printf("[-] Failed to create process! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);
    printf("[*] Attempting NUMA-aware allocation on node 0...\n");

    LPVOID pNumaMemory = VirtualAllocExNuma(
        pi.hProcess,
        NULL,
        4096,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE,
        0  // NUMA node 0
    );

    if (pNumaMemory == NULL) {
        printf("[-] VirtualAllocExNuma failed! Error: %lu\n", GetLastError());
        printf("[*] Note: NUMA may not be available on this system\n");
    } else {
        printf("[+] NUMA memory allocated successfully!\n");
        printf("    Base Address: 0x%p\n", pNumaMemory);
        printf("    NUMA Node: 0\n");
        printf("    Size: 4096 bytes\n");
        PrintProtectionFlags(PAGE_READWRITE);
        printf("\n[*] Use case: Performance optimization on NUMA systems\n");
        VirtualFreeEx(pi.hProcess, pNumaMemory, 0, MEM_RELEASE);
    }

    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("\n[+] Cleanup completed.\n");
}

// ReadProcessMemory - Reads remote process memory
void Demo_ReadProcessMemory() {
    PrintSectionHeader("ReadProcessMemory - Reading Remote Process Memory");

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    printf("[*] Creating target process...\n");
    
    if (!CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED,
        NULL, NULL, &si, &pi)) {
        printf("[-] Failed to create process! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

    // Allocate and write data
    LPVOID pRemote = VirtualAllocEx(pi.hProcess, NULL, 4096,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pRemote == NULL) {
        printf("[-] Memory allocation failed! Error: %lu\n", GetLastError());
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return;
    }

    const char* writeData = "Hello from ReadProcessMemory demo!";
    SIZE_T written;
    
    printf("[*] Writing test data to remote memory...\n");
    if (!WriteProcessMemory(pi.hProcess, pRemote, writeData, strlen(writeData) + 1, &written)) {
        printf("[-] WriteProcessMemory failed! Error: %lu\n", GetLastError());
    } else {
        printf("[+] Wrote %zu bytes to remote address: 0x%p\n", written, pRemote);
        printf("    Data: \"%s\"\n", writeData);
    }

    // Read back the data
    printf("\n[*] Reading data back from remote process...\n");
    char readBuffer[256] = { 0 };
    SIZE_T read;

    if (!ReadProcessMemory(pi.hProcess, pRemote, readBuffer, sizeof(readBuffer), &read)) {
        printf("[-] ReadProcessMemory failed! Error: %lu\n", GetLastError());
    } else {
        printf("[+] Read %zu bytes successfully!\n", read);
        printf("    Data read: \"%s\"\n", readBuffer);
        printf("[+] Data integrity verified!\n");
    }

    // Cleanup
    VirtualFreeEx(pi.hProcess, pRemote, 0, MEM_RELEASE);
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("\n[+] Cleanup completed.\n");
}

// Native APIs - NtAllocateVirtualMemory and others
void Demo_NativeAPIs() {
    PrintSectionHeader("Native APIs - ntdll.dll Memory Functions");

    // Load ntdll.dll and get function addresses
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (hNtdll == NULL) {
        printf("[-] Failed to get ntdll.dll handle!\n");
        return;
    }

    printf("[+] ntdll.dll base address: 0x%p\n\n", hNtdll);

    pNtAllocateVirtualMemory NtAllocateVirtualMemory = 
        (pNtAllocateVirtualMemory)GetProcAddress(hNtdll, "NtAllocateVirtualMemory");
    pNtProtectVirtualMemory NtProtectVirtualMemory = 
        (pNtProtectVirtualMemory)GetProcAddress(hNtdll, "NtProtectVirtualMemory");
    pNtReadVirtualMemory NtReadVirtualMemory = 
        (pNtReadVirtualMemory)GetProcAddress(hNtdll, "NtReadVirtualMemory");
    pNtWriteVirtualMemory NtWriteVirtualMemory = 
        (pNtWriteVirtualMemory)GetProcAddress(hNtdll, "NtWriteVirtualMemory");

    if (!NtAllocateVirtualMemory || !NtProtectVirtualMemory || 
        !NtReadVirtualMemory || !NtWriteVirtualMemory) {
        printf("[-] Failed to get native API addresses!\n");
        return;
    }

    printf("Native API Addresses:\n");
    printf("    NtAllocateVirtualMemory: 0x%p\n", NtAllocateVirtualMemory);
    printf("    NtProtectVirtualMemory:  0x%p\n", NtProtectVirtualMemory);
    printf("    NtReadVirtualMemory:     0x%p\n", NtReadVirtualMemory);
    printf("    NtWriteVirtualMemory:    0x%p\n", NtWriteVirtualMemory);

    // Demonstrate NtAllocateVirtualMemory
    printf("\n[*] Calling NtAllocateVirtualMemory...\n");
    
    PVOID baseAddress = NULL;
    SIZE_T regionSize = 4096;
    
    NTSTATUS status = NtAllocateVirtualMemory(
        GetCurrentProcess(),
        &baseAddress,
        0,
        &regionSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (status == 0) { // STATUS_SUCCESS
        printf("[+] NtAllocateVirtualMemory succeeded!\n");
        printf("    NTSTATUS: 0x%08lX (STATUS_SUCCESS)\n", status);
        printf("    Base Address: 0x%p\n", baseAddress);
        printf("    Region Size: %zu bytes\n", regionSize);

        // Demonstrate NtWriteVirtualMemory
        const char* testData = "Native API test data";
        SIZE_T bytesWritten = 0;
        
        printf("\n[*] Calling NtWriteVirtualMemory...\n");
        status = NtWriteVirtualMemory(
            GetCurrentProcess(),
            baseAddress,
            (PVOID)testData,
            strlen(testData) + 1,
            &bytesWritten
        );

        if (status == 0) {
            printf("[+] NtWriteVirtualMemory succeeded!\n");
            printf("    Bytes written: %zu\n", bytesWritten);

            // Demonstrate NtReadVirtualMemory
            char readBuffer[256] = { 0 };
            SIZE_T bytesRead = 0;
            
            printf("\n[*] Calling NtReadVirtualMemory...\n");
            status = NtReadVirtualMemory(
                GetCurrentProcess(),
                baseAddress,
                readBuffer,
                sizeof(readBuffer),
                &bytesRead
            );

            if (status == 0) {
                printf("[+] NtReadVirtualMemory succeeded!\n");
                printf("    Bytes read: %zu\n", bytesRead);
                printf("    Data: \"%s\"\n", readBuffer);
            }
        }

        // Demonstrate NtProtectVirtualMemory
        printf("\n[*] Calling NtProtectVirtualMemory...\n");
        ULONG oldProtect;
        SIZE_T protectSize = 4096;
        
        status = NtProtectVirtualMemory(
            GetCurrentProcess(),
            &baseAddress,
            &protectSize,
            PAGE_EXECUTE_READ,
            &oldProtect
        );

        if (status == 0) {
            printf("[+] NtProtectVirtualMemory succeeded!\n");
            printf("    Old Protection: 0x%08lX\n", oldProtect);
            printf("    New Protection: PAGE_EXECUTE_READ\n");
        }

        // Cleanup
        VirtualFree(baseAddress, 0, MEM_RELEASE);
        printf("\n[+] Memory freed.\n");
    } else {
        printf("[-] NtAllocateVirtualMemory failed! NTSTATUS: 0x%08lX\n", status);
    }

    printf("\n[*] Native APIs provide lower-level access than kernel32 equivalents\n");
    printf("[*] Used in system programming and security research\n");
}

// 8. VirtualAlloc2 - New allocation API (Windows 10+)
void Demo_VirtualAlloc2() {
    PrintSectionHeader("VirtualAlloc2 - Modern Memory Allocation API");

    printf("[*] Attempting to load VirtualAlloc2 (Windows 10+ only)...\n");

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    pVirtualAlloc2 VirtualAlloc2Func = 
        (pVirtualAlloc2)GetProcAddress(hKernel32, "VirtualAlloc2");

    if (VirtualAlloc2Func == NULL) {
        printf("[-] VirtualAlloc2 not available on this system\n");
        printf("[*] This API requires Windows 10 version 1803 or later\n");
        printf("[*] Using VirtualAlloc as fallback...\n\n");

        // Fallback to regular VirtualAlloc if the VirtualAlloc2Func is not found.
        LPVOID pMemory = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (pMemory) {
            printf("[+] Fallback allocation successful at: 0x%p\n", pMemory);
            VirtualFree(pMemory, 0, MEM_RELEASE);
        }
        return;
    }

    printf("[+] VirtualAlloc2 function found!\n");
    printf("[*] Allocating memory with VirtualAlloc2...\n");

    LPVOID pMemory = VirtualAlloc2Func(
        GetCurrentProcess(),
        NULL,
        4096,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE,
        NULL,
        0
    );

    if (pMemory == NULL) {
        printf("[-] VirtualAlloc2 failed! Error: %lu\n", GetLastError());
        return;
    }

    printf("[+] Memory allocated successfully!\n");
    printf("    Base Address: 0x%p\n", pMemory);
    printf("    Size: 4096 bytes\n");
    PrintProtectionFlags(PAGE_READWRITE);
    printf("\n[*] VirtualAlloc2 provides enhanced control over memory allocation\n");
    printf("[*] Supports extended parameters for advanced scenarios\n");

    VirtualFree(pMemory, 0, MEM_RELEASE);
    printf("\n[+] Memory freed.\n");
}

int main() {
    printf("\n[+]Memory Manipulation APIs - Educational Demonstration\n");
    printf("========================================================\n");

    // Run demonstrations
    Demo_VirtualAlloc();
    Demo_VirtualAllocEx();
    Demo_VirtualProtect();
    Demo_VirtualProtectEx();
    Demo_VirtualAllocExNuma();
    Demo_ReadProcessMemory();
    Demo_NativeAPIs();
    Demo_VirtualAlloc2();

    printf("\n[+]All demonstrations completed!\n");
    printf("\nPress Enter to exit...");
    getchar();

    return 0;
}

