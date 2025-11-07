#include <windows.h>
#include <stdio.h>
#include <winternl.h> // Used for Native APIs.

// Native API definitions
typedef NTSTATUS(NTAPI* pNtQueueApcThread)(
    HANDLE ThreadHandle,
    PVOID ApcRoutine,
    PVOID ApcArgument1,
    PVOID ApcArgument2,
    PVOID ApcArgument3
);

typedef NTSTATUS(NTAPI* pNtQueueApcThreadEx)(
    HANDLE ThreadHandle,
    HANDLE ReserveHandle,
    PVOID ApcRoutine,
    PVOID ApcArgument1,
    PVOID ApcArgument2,
    PVOID ApcArgument3
);

typedef NTSTATUS(NTAPI* pNtQueueApcThreadEx2)(
    HANDLE ThreadHandle,
    HANDLE ReserveHandle,
    ULONG ApcFlags,
    PVOID ApcRoutine,
    PVOID ApcArgument1,
    PVOID ApcArgument2,
    PVOID ApcArgument3
);

// Global function pointers to store the native APIs.
pNtQueueApcThread NtQueueApcThread = NULL;
pNtQueueApcThreadEx NtQueueApcThreadEx = NULL;
pNtQueueApcThreadEx2 NtQueueApcThreadEx2 = NULL;

// Simple APC routine for demonstration
VOID NTAPI SimpleApcRoutine(ULONG_PTR dwParam) {
    printf("[APC] APC executed with parameter: %llu\n", dwParam);
}

// Thread function that enters alertable wait state
DWORD WINAPI AlertableThread(LPVOID lpParam) {
    printf("[Thread] Alertable thread started (TID: %lu)\n", GetCurrentThreadId());
    printf("[Thread] Entering alertable wait state...\n");
    
    // Sleep in alertable state - APCs can execute here
    SleepEx(5000, TRUE);
    
    printf("[Thread] Returned from alertable wait\n");
    return 0;
}

// Initialize Native API function pointers
BOOL InitializeNativeAPIs() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("[-] Failed to get ntdll.dll handle\n");
        return FALSE;
    }

    printf("[+] ntdll.dll base address: 0x%p\n\n", hNtdll);

    // Dynamically get the address of native api functions.
    NtQueueApcThread = (pNtQueueApcThread)GetProcAddress(hNtdll, "NtQueueApcThread");
    NtQueueApcThreadEx = (pNtQueueApcThreadEx)GetProcAddress(hNtdll, "NtQueueApcThreadEx");
    NtQueueApcThreadEx2 = (pNtQueueApcThreadEx2)GetProcAddress(hNtdll, "NtQueueApcThreadEx2");

    printf("Native API Addresses:\n");
    printf("    NtQueueApcThread:    0x%p\n", NtQueueApcThread);
    printf("    NtQueueApcThreadEx:  0x%p\n", NtQueueApcThreadEx);
    printf("    NtQueueApcThreadEx2: 0x%p\n\n", NtQueueApcThreadEx2);

    return TRUE;
}

// Demonstration 1: QueueUserAPC
void Demo_QueueUserAPC() {
    printf("\nQueueUserAPC - Queue APC to Thread\n");

    // Create alertable thread
    HANDLE hThread = CreateThread(NULL, 0, AlertableThread, NULL, 0, NULL);
    if (!hThread) {
        printf("[-] Failed to create thread (Error: %lu)\n\n", GetLastError());
        return;
    }

    printf("[*] Created alertable thread\n");
    Sleep(500); // Let thread start

    // Queue APC to the thread
    printf("[*] Queuing APC using QueueUserAPC...\n");
    DWORD result = QueueUserAPC((PAPCFUNC)SimpleApcRoutine, hThread, 12345);
    
    if (result != 0) {
        printf("[+] QueueUserAPC succeeded!\n");
        printf("    APC queued to thread\n");
        printf("    Will execute when thread enters alertable wait\n\n");
    } else {
        printf("[-] QueueUserAPC failed (Error: %lu)\n\n", GetLastError());
    }

    // Wait for thread to complete
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    printf("[*] Thread completed and cleaned up\n\n");
}

// Demonstration 2: NtQueueApcThread
void Demo_NtQueueApcThread() {
    printf("\nNtQueueApcThread - Native API APC Queueing\n");

    if (!NtQueueApcThread) {
        printf("[-] NtQueueApcThread not available\n\n");
        return;
    }

    // Create alertable thread
    HANDLE hThread = CreateThread(NULL, 0, AlertableThread, NULL, 0, NULL);
    if (!hThread) {
        printf("[-] Failed to create thread (Error: %lu)\n\n", GetLastError());
        return;
    }

    printf("[*] Created alertable thread\n");
    Sleep(500);

    // Queue APC using native API
    printf("[*] Queuing APC using NtQueueApcThread (native API)...\n");
    NTSTATUS status = NtQueueApcThread(
        hThread,
        (PVOID)SimpleApcRoutine,
        (PVOID)67890,
        NULL,
        NULL
    );

    if (status == 0) { // STATUS_SUCCESS
        printf("[+] NtQueueApcThread succeeded!\n");
        printf("    NTSTATUS: 0x%08X (STATUS_SUCCESS)\n", status);
        printf("    Native API successfully queued APC\n\n");
    } else {
        printf("[-] NtQueueApcThread failed\n");
        printf("    NTSTATUS: 0x%08X\n\n", status);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    printf("[*] Thread completed and cleaned up\n\n");
}

// Demonstration 3: NtQueueApcThreadEx
void Demo_NtQueueApcThreadEx() {
    printf("\nNtQueueApcThreadEx - Extended Native APC Queueing\n");

    if (!NtQueueApcThreadEx) {
        printf("[-] NtQueueApcThreadEx not available\n\n");
        return;
    }

    // Create alertable thread
    HANDLE hThread = CreateThread(NULL, 0, AlertableThread, NULL, 0, NULL);
    if (!hThread) {
        printf("[-] Failed to create thread (Error: %lu)\n\n", GetLastError());
        return;
    }

    printf("[*] Created alertable thread\n");
    Sleep(500);

    // Queue APC using extended native API
    printf("[*] Queuing APC using NtQueueApcThreadEx...\n");
    NTSTATUS status = NtQueueApcThreadEx(
        hThread,
        NULL, // ReserveHandle - NULL for user-mode APCs
        (PVOID)SimpleApcRoutine,
        (PVOID)11111,
        NULL,
        NULL
    );

    if (status == 0) {
        printf("[+] NtQueueApcThreadEx succeeded!\n");
        printf("    NTSTATUS: 0x%08X (STATUS_SUCCESS)\n", status);
        printf("    Extended native API queued APC\n");
        printf("    Provides additional control over APC queueing\n\n");
    } else {
        printf("[-] NtQueueApcThreadEx failed\n");
        printf("    NTSTATUS: 0x%08X\n\n", status);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    printf("[*] Thread completed and cleaned up\n\n");
}

// Demonstration 4: NtQueueApcThreadEx2
void Demo_NtQueueApcThreadEx2() {
    printf("\nNtQueueApcThreadEx2 - Newer Extended APC Queueing\n");

    if (!NtQueueApcThreadEx2) {
        printf("[-] NtQueueApcThreadEx2 not available on this system\n");
        printf("[*] This API was introduced in Windows 10 version 1607+\n\n");
        return;
    }

    // Create alertable thread
    HANDLE hThread = CreateThread(NULL, 0, AlertableThread, NULL, 0, NULL);
    if (!hThread) {
        printf("[-] Failed to create thread (Error: %lu)\n\n", GetLastError());
        return;
    }

    printf("[*] Created alertable thread\n");
    Sleep(500);

    // Queue APC using newest native API
    printf("[*] Queuing APC using NtQueueApcThreadEx2...\n");
    NTSTATUS status = NtQueueApcThreadEx2(
        hThread,
        NULL,     // ReserveHandle
        0,        // ApcFlags (0 = user-mode APC)
        (PVOID)SimpleApcRoutine,
        (PVOID)22222,
        NULL,
        NULL
    );

    if (status == 0) {
        printf("[+] NtQueueApcThreadEx2 succeeded!\n");
        printf("    NTSTATUS: 0x%08X (STATUS_SUCCESS)\n", status);
        printf("    Newest extended API with flags support\n");
        printf("    Allows special APC queueing modes\n\n");
    } else {
        printf("[-] NtQueueApcThreadEx2 failed\n");
        printf("    NTSTATUS: 0x%08X\n\n", status);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    printf("[*] Thread completed and cleaned up\n\n");
}

// Demonstration 5: OpenThread for APC injection
void Demo_OpenThread_APC() {
    printf("\nOpenThread - Opening Thread for APC Injection\n");

    // Create a thread to demonstrate opening it
    DWORD dwThreadId = 0;
    HANDLE hOriginalThread = CreateThread(NULL, 0, AlertableThread, NULL, 0, &dwThreadId);
    if (!hOriginalThread) {
        printf("[-] Failed to create thread (Error: %lu)\n\n", GetLastError());
        return;
    }

    printf("[*] Created thread (TID: %lu)\n", dwThreadId);
    Sleep(500);

    // Open the thread using OpenThread
    printf("[*] Opening thread handle using OpenThread...\n");
    HANDLE hThread = OpenThread(
        THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION,
        FALSE,
        dwThreadId
    );

    if (hThread) {
        printf("[+] OpenThread succeeded!\n");
        printf("    Thread handle: 0x%p\n", hThread);
        printf("    Thread ID: %lu\n", dwThreadId);
        printf("    Access rights: THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION\n\n");

        // Queue APC using the opened handle
        printf("[*] Queuing APC to opened thread...\n");
        DWORD result = QueueUserAPC((PAPCFUNC)SimpleApcRoutine, hThread, 99999);
        
        if (result != 0) {
            printf("[+] APC queued successfully using OpenThread handle\n\n");
        } else {
            printf("[-] Failed to queue APC (Error: %lu)\n\n", GetLastError());
        }

        CloseHandle(hThread);
    } else {
        printf("[-] OpenThread failed (Error: %lu)\n\n", GetLastError());
    }

    WaitForSingleObject(hOriginalThread, INFINITE);
    CloseHandle(hOriginalThread);
    
    printf("[*] Thread completed and cleaned up\n\n");
}


int main() 
{
    printf("\nAPC Injection APIs - Educational Demonstration\n");
    printf("This program demonstrates Asynchronous Procedure Call (APC)\n");
    printf("injection techniques using Windows APIs.\n\n");

    // Initialize Native APIs
    if (!InitializeNativeAPIs()) {
        printf("[-] Failed to initialize native APIs\n");
        return 1;
    }

    // Run demonstrations
    Demo_QueueUserAPC();
    Demo_NtQueueApcThread();
    Demo_NtQueueApcThreadEx();
    Demo_NtQueueApcThreadEx2();
    Demo_OpenThread_APC();

    printf("\nAll APC demonstrations completed!\n");

    printf("Press Enter to exit...\n");
    getchar();

    return 0;
}

