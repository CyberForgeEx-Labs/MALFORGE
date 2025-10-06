#include <windows.h>
#include <stdio.h>
#include <winternl.h>

// Define if not available
#ifndef ProcessDebugPort
#define ProcessDebugPort 7
#endif

#ifndef ProcessDebugFlags
#define ProcessDebugFlags 31
#endif

// Function Pointer Setup
typedef NTSTATUS (WINAPI *pNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

// Debugger Checking Function.
void CheckDebugger_NtQueryInformationProcess() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) {
        printf("Failed to get ntdll.dll handle\n");
        return;
    }

    pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
        GetProcAddress(hNtdll, "NtQueryInformationProcess");
    
    if (!NtQIP) {
        printf("Failed to get NtQueryInformationProcess address\n");
        return;
    }

    // Method 1: Check ProcessDebugPort
    // If a debugger is present, the OS returns a non-zero value here.
    DWORD_PTR DebugPort = 0;
    NTSTATUS status = NtQIP(
        // Checking Our Own running program.
        GetCurrentProcess(),
        ProcessDebugPort,
        &DebugPort,
        sizeof(DebugPort),
        NULL
    );

    if (status == 0 && DebugPort != 0) {
        printf("Debugger detected via ProcessDebugPort!\n");
        printf("Debug Port: 0x%p\n", (void*)DebugPort);
        ExitProcess(1);
    }

    // Method 2: Check ProcessDebugFlags
    // If a debugger is present, the OS returns 0 here. If no debugger, it return 1.
    DWORD DebugFlags = 0;
    status = NtQIP(
        GetCurrentProcess(),
        ProcessDebugFlags,
        &DebugFlags,
        sizeof(DebugFlags),
        NULL
    );

    if (status == 0 && DebugFlags == 0) {
        printf("Debugger detected via ProcessDebugFlags!\n");
        printf("Debug Flags: 0x%p\n", DebugFlags);
        ExitProcess(1);
    }

    printf("No debugger detected via NtQueryInformationProcess.\n");
}

int main() {
    printf("Result -> ");
    CheckDebugger_NtQueryInformationProcess();
    return 0;
}