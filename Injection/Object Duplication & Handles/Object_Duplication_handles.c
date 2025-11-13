#include <windows.h>
#include <stdio.h>
#include <winternl.h>

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) // Success label setting
#endif

// Function pointer for NtDuplicateObject
typedef NTSTATUS (NTAPI *pNtDuplicateObject)(
    HANDLE SourceProcessHandle,
    HANDLE SourceHandle,
    HANDLE TargetProcessHandle,
    PHANDLE TargetHandle,
    ACCESS_MASK DesiredAccess,
    ULONG HandleAttributes,
    ULONG Options
);

// Options for NtDuplicateObject
#define DUPLICATE_CLOSE_SOURCE 0x00000001
#define DUPLICATE_SAME_ACCESS  0x00000002

/*
 * NtDuplicateObject
 * Purpose: Duplicates an object handle from one process to another
*/
void Demo_NtDuplicateObject() {
    printf("\nNtDuplicateObject Demonstration\n");
    
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[!] Failed to get ntdll.dll handle\n");
        return;
    }
    
    pNtDuplicateObject NtDuplicateObject = (pNtDuplicateObject)GetProcAddress(hNtdll, "NtDuplicateObject");
    if (!NtDuplicateObject) {
        printf("[!] Failed to get NtDuplicateObject address\n");
        return;
    }
    
    printf("[+] NtDuplicateObject located at: 0x%p\n", NtDuplicateObject);
    
    // Create a sample event object to duplicate
    HANDLE hOriginalEvent = CreateEventA(NULL, TRUE, FALSE, "TestEvent");
    if (!hOriginalEvent) {
        printf("[!] Failed to create event object. Error: %lu\n", GetLastError());
        return;
    }
    printf("[+] Created original event handle: 0x%p\n", hOriginalEvent);
    
    // Duplicate the handle within the same process
    HANDLE hDuplicatedEvent = NULL;
    HANDLE hCurrentProcess = GetCurrentProcess();
    
    NTSTATUS status = NtDuplicateObject(
        hCurrentProcess,              // Source process
        hOriginalEvent,               // Source handle
        hCurrentProcess,              // Target process (same)
        &hDuplicatedEvent,            // Target handle
        0,                            // Desired access (0 = same as source)
        0,                            // Handle attributes
        DUPLICATE_SAME_ACCESS         // Options
    );
    
    if (status == STATUS_SUCCESS && hDuplicatedEvent) {
        printf("[+] Successfully duplicated handle: 0x%p -> 0x%p\n", 
               hOriginalEvent, hDuplicatedEvent);
        printf("[+] Both handles reference the same kernel object\n");
        
        // Test both handles work
        SetEvent(hOriginalEvent);
        if (WaitForSingleObject(hDuplicatedEvent, 0) == WAIT_OBJECT_0) {
            printf("[+] Verification: Duplicated handle correctly references same event\n");
        }
        
        CloseHandle(hDuplicatedEvent);
    } else {
        printf("[!] NtDuplicateObject failed. NTSTATUS: 0x%08X\n", status);
    }
    
    CloseHandle(hOriginalEvent);
}

/*
 * Advanced Handle Duplication
 * Purpose: Shows cross-process handle duplication scenario
 */
void Demo_CrossProcessDuplication() {
    printf("\n Cross-Process Handle Duplication Demonstration\n");
    
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[!] Failed to get ntdll.dll handle\n");
        return;
    }
    
    pNtDuplicateObject NtDuplicateObject = (pNtDuplicateObject)GetProcAddress(hNtdll, "NtDuplicateObject");
    if (!NtDuplicateObject) {
        printf("[!] Failed to get NtDuplicateObject address\n");
        return;
    }
    
    // Create a mutex for testing
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "TestMutex");
    if (!hMutex) {
        printf("[!] Failed to create mutex. Error: %lu\n", GetLastError());
        return;
    }
    printf("[+] Created mutex handle: 0x%p\n", hMutex);
    
    // Spawn a child process (notepad as example)
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    
    if (CreateProcessA("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, 
                       FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("[+] Spawned child process PID: %lu\n", pi.dwProcessId);
        
        // Duplicate handle into child process
        HANDLE hRemoteMutex = NULL;
        NTSTATUS status = NtDuplicateObject(
            GetCurrentProcess(),      // Source process (current)
            hMutex,                   // Source handle
            pi.hProcess,              // Target process (child)
            &hRemoteMutex,            // Target handle
            SYNCHRONIZE,              // Desired access
            0,                        // Handle attributes
            0                         // Options
        );
        
        if (status == STATUS_SUCCESS) {
            printf("[+] Successfully duplicated handle into remote process\n");
            printf("[+] Remote handle value: 0x%p\n", hRemoteMutex);
            printf("[+] Child process can now access the mutex object\n");
        } else {
            printf("[!] Cross-process duplication failed. NTSTATUS: 0x%08X\n", status);
        }
        
        // Clean up child process
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        printf("[!] Failed to create child process. Error: %lu\n", GetLastError());
    }
    
    CloseHandle(hMutex);

}

/*
 * Handle Information Query
 */
void Demo_HandleInformation() {
    printf("\n Handle Information Query Demonstration \n");
    
    HANDLE hFile = CreateFileA("test_handle.tmp", GENERIC_WRITE, 0, NULL, 
                               CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 
                               NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("[!] Failed to create file. Error: %lu\n", GetLastError());
        return;
    }
    
    printf("[+] Created file handle: 0x%p\n", hFile);
    
    // Duplicate with close source option
    HANDLE hDupFile = NULL;
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    pNtDuplicateObject NtDuplicateObject = (pNtDuplicateObject)GetProcAddress(hNtdll, "NtDuplicateObject");
    
    if (NtDuplicateObject) {
        NTSTATUS status = NtDuplicateObject(
            GetCurrentProcess(),
            hFile,
            GetCurrentProcess(),
            &hDupFile,
            0,
            0,
            DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE
        );
        
        if (status == STATUS_SUCCESS) {
            printf("[+] Duplicated handle with DUPLICATE_CLOSE_SOURCE\n");
            printf("[+] Original handle closed automatically: 0x%p\n", hFile);
            printf("[+] New handle: 0x%p\n", hDupFile);
            
            // Original handle is now invalid
            DWORD bytesWritten;
            if (!WriteFile(hFile, "test", 4, &bytesWritten, NULL)) {
                printf("[+] Verified: Original handle is now invalid (Error: %lu)\n", GetLastError());
            }
            
            // But duplicated handle works
            if (WriteFile(hDupFile, "test", 4, &bytesWritten, NULL)) {
                printf("[+] Verified: Duplicated handle is valid and working\n");
            }
            
            CloseHandle(hDupFile);
        }
    }
    
}


int main() 
{
    printf("\nObject Duplication & Handle Management\n");
    
    // Run demonstrations
    Demo_NtDuplicateObject();
    Demo_CrossProcessDuplication();
    Demo_HandleInformation();
    
    printf("Demonstration Complete\n");

    
    printf("\n[*] Press Enter to exit...");
    getchar();
    
    return 0;
}