#include <windows.h>
#include <stdio.h>
#include <winternl.h>

// Function prototypes for Native APIs (ntdll)
typedef NTSTATUS(NTAPI* pNtUnmapViewOfSection)(HANDLE, PVOID);
typedef NTSTATUS(NTAPI* pNtResumeThread)(HANDLE, PULONG);
typedef NTSTATUS(NTAPI* pNtSuspendProcess)(HANDLE);

// Helper function to print error messages
void PrintError(const char* operation) {
    DWORD error = GetLastError();
    printf("[-] %s failed with error code: %lu\n", operation, error);
}

// CreateProcessA - Creating a suspended process
void Demo_CreateProcessA() {
    printf("\nCreateProcessA - Creating Suspended Process\n");

    STARTUPINFOA si = { 0 }; // Zeroing Out
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    // Create notepad in suspended state for educational purposes
    printf("[*] Creating notepad.exe in SUSPENDED state...\n");
    
    if (CreateProcessA(
        NULL,
        "notepad.exe",
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,  // Process starts suspended
        NULL,
        NULL,
        &si,
        &pi
    )) {
        printf("[+] Process created successfully!\n");
        printf("    Process ID: %lu\n", pi.dwProcessId);
        printf("    Thread ID: %lu\n", pi.dwThreadId);
        printf("    Process Handle: 0x%p\n", pi.hProcess);
        printf("    Thread Handle: 0x%p\n", pi.hThread);
        printf("    State: SUSPENDED (not running yet)\n\n");

        printf("[*] Process is suspended. It will not execute until ResumeThread is called.\n");
        printf("[*] Terminating suspended process for cleanup...\n");
        
        // Cleanup - terminate the suspended process
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        printf("[+] Cleanup completed.\n");
    } else {
        PrintError("CreateProcessA");
    }
}

// GetThreadContext - Reading thread registers
void Demo_GetThreadContext() {
    printf("\nGetThreadContext - Reading Thread CPU Context\n");

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    printf("[*] Creating suspended process to demonstrate GetThreadContext...\n");
    
    if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, 
                       CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        
        printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

        CONTEXT ctx = { 0 };
        ctx.ContextFlags = CONTEXT_ALL;

        printf("[*] Calling GetThreadContext to read CPU registers...\n");
        
        if (GetThreadContext(pi.hThread, &ctx)) {
            printf("[+] Thread context retrieved successfully!\n\n");
            printf("CPU Register Values:\n");
            
#ifdef _WIN64
            printf("    RIP (Instruction Pointer): 0x%llx\n", ctx.Rip);
            printf("    RSP (Stack Pointer):       0x%llx\n", ctx.Rsp);
            printf("    RBP (Base Pointer):        0x%llx\n", ctx.Rbp);
            printf("    RAX:                       0x%llx\n", ctx.Rax);
            printf("    RBX:                       0x%llx\n", ctx.Rbx);
            printf("    RCX:                       0x%llx\n", ctx.Rcx);
#else
            printf("    EIP (Instruction Pointer): 0x%08lx\n", ctx.Eip);
            printf("    ESP (Stack Pointer):       0x%08lx\n", ctx.Esp);
            printf("    EBP (Base Pointer):        0x%08lx\n", ctx.Ebp);
            printf("    EAX:                       0x%08lx\n", ctx.Eax);
            printf("    EBX:                       0x%08lx\n", ctx.Ebx);
            printf("    ECX:                       0x%08lx\n", ctx.Ecx);
#endif
            printf("\n[*] These registers show where execution will start when thread resumes.\n");
        } else {
            PrintError("GetThreadContext");
        }

        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        PrintError("CreateProcessA");
    }
}

// SetThreadContext - Modifying thread registers
void Demo_SetThreadContext() {
    printf("\nSetThreadContext - Modifying Thread CPU Context\n");

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    printf("[*] Creating suspended process...\n");
    
    if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, 
                       CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        
        printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

        CONTEXT ctx = { 0 };
        ctx.ContextFlags = CONTEXT_ALL;

        if (GetThreadContext(pi.hThread, &ctx)) {
#ifdef _WIN64
            ULONGLONG originalRip = ctx.Rip;
            printf("[*] Original RIP: 0x%llx\n", originalRip);
            
            // Demonstrate modification (not actually changing execution)
            printf("[*] Demonstrating SetThreadContext (no actual modification)...\n");
#else
            DWORD originalEip = ctx.Eip;
            printf("[*] Original EIP: 0x%08lx\n", originalEip);
            
            printf("[*] Demonstrating SetThreadContext (no actual modification)...\n");
#endif
            
            // We could modify ctx.Rip/Eip here, but we won't for safety
            // Basically it redirecting the code execution.
            if (SetThreadContext(pi.hThread, &ctx)) {
                printf("[+] SetThreadContext succeeded!\n");
                printf("[*] In real scenarios, this API can redirect thread execution.\n");
                printf("[*] Used legitimately in debuggers and exception handlers.\n");
            } else {
                PrintError("SetThreadContext");
            }
        }

        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        PrintError("CreateProcessA");
    }
}

// SuspendThread and ResumeThread
void Demo_SuspendResumeThread() {
    printf("\nSuspendThread / ResumeThread - Thread State Control\n");

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    printf("[*] Creating process in normal (running) state...\n");
    
    if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, 
                       0, NULL, NULL, &si, &pi)) {
        
        printf("[+] Process created and running (PID: %lu)\n", pi.dwProcessId);
        printf("[*] Process is currently executing normally...\n\n");

        Sleep(500); // Let it run briefly

        printf("[*] Calling SuspendThread...\n");
        DWORD suspendCount = SuspendThread(pi.hThread);
        
        if (suspendCount != (DWORD)-1) {
            printf("[+] Thread suspended! Suspend count: %lu\n", suspendCount);
            printf("    The process is now frozen and not executing.\n\n");

            Sleep(1000); // Keep it suspended

            printf("[*] Calling ResumeThread...\n");
            DWORD resumeCount = ResumeThread(pi.hThread);
            
            if (resumeCount != (DWORD)-1) {
                printf("[+] Thread resumed! Previous suspend count: %lu\n", resumeCount);
                printf("    The process is now executing again.\n\n");
            } else {
                PrintError("ResumeThread");
            }

            Sleep(500);
        } else {
            PrintError("SuspendThread");
        }

        printf("[*] Terminating process for cleanup...\n");
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        printf("[+] Cleanup completed.\n");
    } else {
        PrintError("CreateProcessA");
    }
}

// VirtualAllocEx - Remote memory allocation
// Allocating the memory in the process.
void Demo_VirtualAllocEx() {
    printf("\nVirtualAllocEx - Allocating Memory in Remote Process\n");

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    printf("[*] Creating target process...\n");
    
    if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, 
                       CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        
        printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

        SIZE_T allocSize = 4096; // 1 page
        printf("[*] Allocating %zu bytes in remote process...\n", allocSize);

        LPVOID remoteAddr = VirtualAllocEx(
            pi.hProcess,
            NULL,
            allocSize,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE
        );

        if (remoteAddr) {
            printf("[+] Memory allocated successfully!\n");
            printf("    Base Address: 0x%p\n", remoteAddr);
            printf("    Size: %zu bytes\n", allocSize);
            printf("    Protection: PAGE_READWRITE\n");
            printf("    Type: MEM_COMMIT | MEM_RESERVE\n\n");

            printf("[*] This memory exists in the target process's address space.\n");
            printf("[*] Can be written to using WriteProcessMemory.\n");

            // Cleanup
            VirtualFreeEx(pi.hProcess, remoteAddr, 0, MEM_RELEASE);
            printf("[*] Memory freed.\n");
        } else {
            PrintError("VirtualAllocEx");
        }

        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        PrintError("CreateProcessA");
    }
}

// WriteProcessMemory
void Demo_WriteProcessMemory() {
    printf("\nWriteProcessMemory - Writing Data to Remote Process\n");

    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    printf("[*] Creating target process...\n");
    
    if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, 
                       CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        
        printf("[+] Process created (PID: %lu)\n", pi.dwProcessId);

        // Allocate memory in target process
        LPVOID remoteAddr = VirtualAllocEx(pi.hProcess, NULL, 4096, 
                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (remoteAddr) {
            printf("[+] Remote memory allocated at: 0x%p\n", remoteAddr);

            // Data to write
            const char* testData = "Hello from WriteProcessMemory!"; //sample data to write.
            SIZE_T dataLen = strlen(testData) + 1;
            SIZE_T bytesWritten = 0;

            printf("[*] Writing %zu bytes to remote process...\n", dataLen);
            printf("    Data: \"%s\"\n", testData);

            if (WriteProcessMemory(pi.hProcess, remoteAddr, testData, 
                                  dataLen, &bytesWritten)) {
                printf("[+] Write successful!\n");
                printf("    Bytes written: %zu\n", bytesWritten);
                printf("    Target address: 0x%p\n\n", remoteAddr);

                // Read it back to verify
                char buffer[256] = { 0 };
                SIZE_T bytesRead = 0;

                if (ReadProcessMemory(pi.hProcess, remoteAddr, buffer, 
                                     dataLen, &bytesRead)) {
                    printf("[*] Verification read:\n");
                    printf("    Bytes read: %zu\n", bytesRead);
                    printf("    Data read back: \"%s\"\n", buffer);
                    printf("[+] Data integrity verified!\n");
                }
            } else {
                PrintError("WriteProcessMemory");
            }

            VirtualFreeEx(pi.hProcess, remoteAddr, 0, MEM_RELEASE);
        } else {
            PrintError("VirtualAllocEx");
        }

        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        PrintError("CreateProcessA");
    }
}

// Native APIs (NtUnmapViewOfSection, NtResumeThread, NtSuspendProcess)
void Demo_NativeAPIs() {
    printf("\nNative APIs - ntdll.dll Function Demonstration\n");

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[-] Failed to get ntdll.dll handle\n");
        return;
    }

    printf("[+] ntdll.dll base address: 0x%p\n\n", hNtdll);

    // Get function addresses
    pNtUnmapViewOfSection NtUnmapViewOfSection = 
        (pNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
    pNtResumeThread NtResumeThread = 
        (pNtResumeThread)GetProcAddress(hNtdll, "NtResumeThread");
    pNtSuspendProcess NtSuspendProcess = 
        (pNtSuspendProcess)GetProcAddress(hNtdll, "NtSuspendProcess");

    printf("Native API Addresses:\n");
    printf("    NtUnmapViewOfSection: 0x%p\n", NtUnmapViewOfSection);
    printf("    NtResumeThread:       0x%p\n", NtResumeThread);
    printf("    NtSuspendProcess:     0x%p\n\n", NtSuspendProcess);

    if (!NtUnmapViewOfSection || !NtResumeThread || !NtSuspendProcess) {
        printf("[-] Failed to resolve one or more native APIs\n");
        return;
    }

    printf("[*] These are semi-documented native APIs from ntdll.dll\n");
    printf("[*] They provide lower-level access than kernel32.dll functions.\n\n");

    printf("API Purposes:\n");
    printf("    - NtUnmapViewOfSection: Unmaps a section from process address space\n");
    printf("    - NtResumeThread:       Native version of ResumeThread\n");
    printf("    - NtSuspendProcess:     Suspends entire process (all threads)\n\n");

    // Demonstrate NtResumeThread
    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    if (CreateProcessA(NULL, "notepad.exe", NULL, NULL, FALSE, 
                       CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        
        printf("[*] Created suspended process (PID: %lu)\n", pi.dwProcessId);
        printf("[*] Calling NtResumeThread (native API)...\n");

        ULONG suspendCount = 0;
        NTSTATUS status = NtResumeThread(pi.hThread, &suspendCount);

        if (status == 0) { // STATUS_SUCCESS
            printf("[+] NtResumeThread succeeded!\n");
            printf("    NTSTATUS: 0x%08lx (STATUS_SUCCESS)\n", status);
            printf("    Previous suspend count: %lu\n\n", suspendCount);

            Sleep(500);

            // Demonstrate NtSuspendProcess
            printf("[*] Calling NtSuspendProcess...\n");
            status = NtSuspendProcess(pi.hProcess);

            if (status == 0) {
                printf("[+] NtSuspendProcess succeeded!\n");
                printf("    NTSTATUS: 0x%08lx (STATUS_SUCCESS)\n", status);
                printf("    Entire process is now suspended.\n");
            } else {
                printf("[-] NtSuspendProcess failed with NTSTATUS: 0x%08lx\n", status);
            }
        } else {
            printf("[-] NtResumeThread failed with NTSTATUS: 0x%08lx\n", status);
        }

        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main() 
{
    printf("\nWindows Process Management APIs - Demonstration.\n");
    printf("This program demonstrates legitimate uses of Windows process\n");
    printf("\n");
    printf("NOTE: Processes created are immediately terminated for cleanup.\n");
    printf("      No harmful operations are performed.\n");

    // Run all demonstrations
    Demo_CreateProcessA();
    Demo_GetThreadContext();
    Demo_SetThreadContext();
    Demo_SuspendResumeThread();
    Demo_VirtualAllocEx();
    Demo_WriteProcessMemory();
    Demo_NativeAPIs();

    printf("\nAll demonstrations completed successfully!\n");

    return 0;
}