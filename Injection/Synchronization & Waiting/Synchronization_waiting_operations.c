#include <windows.h>
#include <stdio.h>

// Native API declarations
typedef LONG NTSTATUS;

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#define STATUS_TIMEOUT ((NTSTATUS)0x00000102L)

#define NUM_EVENTS 3

typedef enum _WAIT_TYPE {
    WaitAll,
    WaitAny
} WAIT_TYPE;

typedef NTSTATUS (NTAPI *pNtWaitForSingleObject)(
    HANDLE Handle,
    BOOLEAN Alertable,
    PLARGE_INTEGER Timeout
);

typedef NTSTATUS (NTAPI *pNtWaitForMultipleObjects)(
    ULONG Count,
    HANDLE Handles[],
    WAIT_TYPE WaitType,
    BOOLEAN Alertable,
    PLARGE_INTEGER Timeout
);

// output helpers.
void PrintSuccess(const char* msg) {
    printf("[+] %s\n", msg);
}

void PrintError(const char* msg) {
    printf("[-] %s (Error: %lu)\n", msg, GetLastError());
}

void PrintInfo(const char* msg) {
    printf("[*] %s\n", msg);
}

// Thread function for demonstrations
DWORD WINAPI WorkerThread(LPVOID lpParam) {

    HANDLE hEvent = (HANDLE)lpParam;
    
    PrintInfo("Worker thread started, performing task...");
    Sleep(2000); // Simulate work
    
    if (hEvent) {
        SetEvent(hEvent);
        PrintSuccess("Worker thread completed, event signaled");
    }
    
    return 0;
}

// WaitForSingleObject demonstration
void Demo_WaitForSingleObject() {
    printf("\nWaitForSingleObject Demonstration.\n");
    
    // Create an event
    HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent1");
    if (!hEvent) {
        PrintError("Failed to create event");
        return;
    }
    PrintSuccess("Event created");
    
    // Create worker thread
    HANDLE hThread = CreateThread(NULL, 0, WorkerThread, hEvent, 0, NULL);
    if (!hThread) {
        PrintError("Failed to create thread");
        CloseHandle(hEvent);
        return;
    }
    PrintSuccess("Worker thread created");
    
    // Wait for event to be signaled
    PrintInfo("Main thread waiting for event (timeout: 5000ms)...");
    DWORD dwResult = WaitForSingleObject(hEvent, 5000);
    
    switch (dwResult) {
        case WAIT_OBJECT_0:
            PrintSuccess("Event was signaled - thread completed successfully");
            break;
        case WAIT_TIMEOUT:
            PrintError("Wait timed out");
            break;
        case WAIT_ABANDONED:
            PrintError("Wait was abandoned");
            break;
        case WAIT_FAILED:
            PrintError("WaitForSingleObject failed");
            break;
    }
    
    // Cleanup
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hEvent);
    PrintInfo("Resources cleaned up");
}

// WaitForSingleObjectEx with alertable wait
void Demo_WaitForSingleObjectEx() {
    printf("\nWaitForSingleObjectEx (Alertable) Demonstration.\n");
    
    HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent2");
    if (!hEvent) {
        PrintError("Failed to create event");
        return;
    }
    PrintSuccess("Event created");
    
    // Create thread that will signal after delay
    HANDLE hThread = CreateThread(NULL, 0, WorkerThread, hEvent, 0, NULL);
    if (!hThread) {
        PrintError("Failed to create thread");
        CloseHandle(hEvent);
        return;
    }
    
    // Alertable wait - can be interrupted by APCs
    PrintInfo("Performing alertable wait (allows APC delivery)...");
    DWORD dwResult = WaitForSingleObjectEx(hEvent, 5000, TRUE);
    
    switch (dwResult) {
        case WAIT_OBJECT_0:
            PrintSuccess("Event signaled");
            break;
        case WAIT_IO_COMPLETION:
            PrintInfo("Wait interrupted by I/O completion (APC)");
            break;
        case WAIT_TIMEOUT:
            PrintError("Wait timed out");
            break;
        case WAIT_FAILED:
            PrintError("WaitForSingleObjectEx failed");
            break;
    }
    
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hEvent);
}

// WaitForMultipleObjects Demonstrations.
void Demo_WaitForMultipleObjects() {
    printf("\nWaitForMultipleObjects Demonstrations\n");
    
    HANDLE hEvents[NUM_EVENTS];
    HANDLE hThreads[NUM_EVENTS];
    int i, j;
    
    // Create multiple events and threads
    for (i = 0; i < NUM_EVENTS; i++) {
        wchar_t eventName[50];
        swprintf(eventName, 50, L"DemoEvent3_%d", i);
        
        hEvents[i] = CreateEventW(NULL, TRUE, FALSE, eventName);
        if (!hEvents[i]) {
            PrintError("Failed to create event");
            // Cleanup previously created events
            for (j = 0; j < i; j++) {
                CloseHandle(hEvents[j]);
            }
            return;
        }
        
        // Create thread with staggered delays
        hThreads[i] = CreateThread(NULL, 0, WorkerThread, hEvents[i], 0, NULL);
        Sleep(500); // Stagger thread creation
    }
    
    PrintInfo("Waiting for ALL events to be signaled...");
    
    // Wait for all objects
    DWORD dwResult = WaitForMultipleObjects(NUM_EVENTS, hEvents, TRUE, 10000);
    
    if (dwResult >= WAIT_OBJECT_0 && dwResult < WAIT_OBJECT_0 + NUM_EVENTS) {
        PrintSuccess("All events signaled");
    } else if (dwResult == WAIT_TIMEOUT) {
        PrintError("Wait timed out");
    } else if (dwResult == WAIT_FAILED) {
        PrintError("WaitForMultipleObjects failed");
    }
    
    // Now demonstrate wait for ANY
    ResetEvent(hEvents[0]);
    SetEvent(hEvents[1]);
    
    PrintInfo("Waiting for ANY event to be signaled...");
    dwResult = WaitForMultipleObjects(NUM_EVENTS, hEvents, FALSE, 1000);
    
    if (dwResult >= WAIT_OBJECT_0 && dwResult < WAIT_OBJECT_0 + NUM_EVENTS) {
        printf("[+] Event %lu was signaled\n", dwResult - WAIT_OBJECT_0);
    }
    
    // Cleanup
    for (i = 0; i < NUM_EVENTS; i++) {
        WaitForSingleObject(hThreads[i], INFINITE);
        CloseHandle(hThreads[i]);
        CloseHandle(hEvents[i]);
    }
    PrintInfo("All resources cleaned up");
}

// WaitForMultipleObjectsEx
void Demo_WaitForMultipleObjectsEx() {
    printf("\n WaitForMultipleObjectsEx Demonstration.\n");
    
    HANDLE hEvents[2];
    
    hEvents[0] = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent4_A");
    hEvents[1] = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent4_B");
    
    if (!hEvents[0] || !hEvents[1]) {
        PrintError("Failed to create events");
        return;
    }
    
    // Create threads
    HANDLE hThread1 = CreateThread(NULL, 0, WorkerThread, hEvents[0], 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, WorkerThread, hEvents[1], 0, NULL);
    
    // Alertable wait for multiple objects
    PrintInfo("Alertable wait for multiple objects...");
    DWORD dwResult = WaitForMultipleObjectsEx(2, hEvents, TRUE, 5000, TRUE);
    
    switch (dwResult) {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0 + 1:
            PrintSuccess("Objects signaled");
            break;
        case WAIT_IO_COMPLETION:
            PrintInfo("Wait interrupted by I/O completion");
            break;
        case WAIT_TIMEOUT:
            PrintError("Wait timed out");
            break;
        case WAIT_FAILED:
            PrintError("WaitForMultipleObjectsEx failed");
            break;
    }
    
    // Cleanup
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(hEvents[0]);
    CloseHandle(hEvents[1]);
}

// NtWaitForSingleObject
void Demo_NtWaitForSingleObject() {
    printf("\nNtWaitForSingleObject (Native API) Demonstration.\n");
    
    // Load native API
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        PrintError("Failed to get ntdll.dll handle");
        return;
    }
    
    pNtWaitForSingleObject NtWaitForSingleObject = 
        (pNtWaitForSingleObject)GetProcAddress(hNtdll, "NtWaitForSingleObject");
    
    if (!NtWaitForSingleObject) {
        PrintError("Failed to get NtWaitForSingleObject address");
        return;
    }
    
    // Create event
    HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent5");
    if (!hEvent) {
        PrintError("Failed to create event");
        return;
    }
    
    HANDLE hThread = CreateThread(NULL, 0, WorkerThread, hEvent, 0, NULL);
    
    // Setup timeout (5 seconds in 100-nanosecond intervals, negative for relative)
    LARGE_INTEGER timeout;
    timeout.QuadPart = -50000000LL; // 5 seconds
    
    PrintInfo("Native API wait with 5 second timeout...");
    NTSTATUS status = NtWaitForSingleObject(hEvent, FALSE, &timeout);
    
    if (status == STATUS_SUCCESS) {
        PrintSuccess("Object signaled (STATUS_SUCCESS)");
    } else if (status == STATUS_TIMEOUT) {
        PrintError("Wait timed out (STATUS_TIMEOUT)");
    } else {
        printf("[-] NtWaitForSingleObject returned: 0x%08X\n", status);
    }
    
    // Cleanup
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hEvent);
}

// NtWaitForMultipleObjects
void Demo_NtWaitForMultipleObjects() {
    printf("\n NtWaitForMultipleObjects (Native API) Demonstration.\n");
    
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        PrintError("Failed to get ntdll.dll handle");
        return;
    }
    
    pNtWaitForMultipleObjects NtWaitForMultipleObjects = 
        (pNtWaitForMultipleObjects)GetProcAddress(hNtdll, "NtWaitForMultipleObjects");
    
    if (!NtWaitForMultipleObjects) {
        PrintError("Failed to get NtWaitForMultipleObjects address");
        return;
    }
    
    // Create events
    HANDLE hEvents[2];
    hEvents[0] = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent6_A");
    hEvents[1] = CreateEventW(NULL, TRUE, FALSE, L"DemoEvent6_B");
    
    if (!hEvents[0] || !hEvents[1]) {
        PrintError("Failed to create events");
        return;
    }
    
    HANDLE hThread1 = CreateThread(NULL, 0, WorkerThread, hEvents[0], 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, WorkerThread, hEvents[1], 0, NULL);
    
    LARGE_INTEGER timeout;
    timeout.QuadPart = -50000000LL; // 5 seconds
    
    PrintInfo("Native API waiting for all objects...");
    NTSTATUS status = NtWaitForMultipleObjects(
        2,              // Count
        hEvents,        // Handles array
        WaitAll,        // Wait type
        FALSE,          // Not alertable
        &timeout        // Timeout
    );
    
    if (status == STATUS_SUCCESS) {
        PrintSuccess("All objects signaled");
    } else if (status == STATUS_TIMEOUT) {
        PrintError("Wait timed out");
    } else {
        printf("[-] NtWaitForMultipleObjects returned: 0x%08X\n", status);
    }
    
    // Cleanup
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(hEvents[0]);
    CloseHandle(hEvents[1]);
}

// Process synchronization example.
void Demo_ProcessSynchronization() {
    printf("\n Process Synchronization\n");
    
    // Create a child process
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    wchar_t cmdLine[] = L"notepad.exe";
    
    PrintInfo("Creating child process (notepad.exe)...");
    if (!CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        PrintError("Failed to create process");
        return;
    }
    
    PrintSuccess("Process created");
    printf("[*] Process ID: %lu\n", pi.dwProcessId);
    printf("[*] Thread ID: %lu\n", pi.dwThreadId);
    
    // Wait for process initialization (thread handle)
    PrintInfo("Waiting for process initialization...");
    DWORD result = WaitForSingleObject(pi.hThread, 2000);
    
    if (result == WAIT_TIMEOUT) {
        PrintSuccess("Process initialized and running");
    }
    
    // Demonstrate waiting for process termination
    PrintInfo("Press Enter to terminate and wait for process exit...");
    getchar();
    
    TerminateProcess(pi.hProcess, 0);
    
    PrintInfo("Waiting for process termination...");
    result = WaitForSingleObject(pi.hProcess, 5000);
    
    if (result == WAIT_OBJECT_0) {
        PrintSuccess("Process terminated successfully");
        
        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            printf("[*] Exit code: %lu\n", exitCode);
        }
    } else if (result == WAIT_TIMEOUT) {
        PrintError("Process did not terminate within timeout");
    }
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() 
{
    printf("\nWindows Synchronization & Wait Operations Demonstration\n");
    
    
    // Run demonstrations
    Demo_WaitForSingleObject();
    Demo_WaitForSingleObjectEx();
    Demo_WaitForMultipleObjects();
    Demo_WaitForMultipleObjectsEx();
    Demo_NtWaitForSingleObject();
    Demo_NtWaitForMultipleObjects();
    Demo_ProcessSynchronization();
    
    printf("\n\nAll demonstrations completed!\n");

    
    return 0;
}