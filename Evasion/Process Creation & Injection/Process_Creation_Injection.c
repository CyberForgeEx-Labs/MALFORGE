#include <windows.h>
#include <stdio.h>

// Typedef for CreateProcessInternalW (undocumented API).
typedef DWORD (WINAPI *pCreateProcessInternalW)(
    DWORD unknown1,                    // Usually 0
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation,
    DWORD unknown2                     // Usually 0
);

int main() 
{
    // Load kernel32.dll and get address of CreateProcessInternalW
    HMODULE hKernel32 = LoadLibrary("kernel32.dll");
    if (hKernel32 == NULL) {
        printf("[ERROR] Failed to load kernel32.dll\n");
        return 1;
    }

    pCreateProcessInternalW CreateProcessInternalW = (pCreateProcessInternalW)GetProcAddress(hKernel32, "CreateProcessInternalW");
    if (CreateProcessInternalW == NULL) {
        printf("[ERROR] Failed to get CreateProcessInternalW address\n");
        FreeLibrary(hKernel32);
        return 1;
    }

    // Prepare parameters for process creation (e.g., launch notepad.exe suspended)
    wchar_t appName[] = L"C:\\Windows\\System32\\notepad.exe";
    wchar_t cmdLine[] = L"notepad.exe";  // Or NULL if appName includes args

    STARTUPINFOW si = {0};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi = {0};

    // Create suspended process
    DWORD creationFlags = CREATE_SUSPENDED;  // Suspend to allow injection

    DWORD result = CreateProcessInternalW(
        0,                  // unknown1
        appName,
        cmdLine,
        NULL,               // Process attributes
        NULL,               // Thread attributes
        FALSE,              // Inherit handles
        creationFlags,
        NULL,               // Environment
        NULL,               // Current directory
        &si,
        &pi,
        0                   // unknown2
    );

    if (result == 0) {
        printf("[ERROR] CreateProcessInternalW failed with error: %lu\n", GetLastError());
        FreeLibrary(hKernel32);
        return 1;
    }

    printf("[+] Process created successfully. PID: %lu\n", pi.dwProcessId);

    // Example: DLL Injection into the new process
    // For demonstration, inject a DLL (e.g., "C:\\path\\to\\mydll.dll")
    wchar_t dllPath[] = L"DLL_NAME";  // Use a real DLL path for testing.

    // Allocate memory in target process for DLL path
    SIZE_T dllPathLen = (wcslen(dllPath) + 1) * sizeof(wchar_t);
    LPVOID remoteAddr = VirtualAllocEx(pi.hProcess, NULL, dllPathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteAddr == NULL) {
        printf("[ERROR] VirtualAllocEx failed: %lu\n", GetLastError());
        goto cleanup;
    }

    // Write DLL path to remote process
    if (!WriteProcessMemory(pi.hProcess, remoteAddr, dllPath, dllPathLen, NULL)) {
        printf("[ERROR] WriteProcessMemory failed: %lu\n", GetLastError());
        goto cleanup;
    }

    // Get address of LoadLibraryW in kernel32.dll
    HMODULE hKernel32Remote = GetModuleHandle("kernel32.dll");  // Local, assume same in remote
    FARPROC loadLibraryAddr = GetProcAddress(hKernel32Remote, "LoadLibraryW");

    // Create remote thread to call LoadLibraryW with the DLL path
    HANDLE hRemoteThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteAddr, 0, NULL);
    if (hRemoteThread == NULL) {
        printf("[ERROR] CreateRemoteThread failed: %lu\n", GetLastError());
        goto cleanup;
    }

    // Wait for injection thread to finish
    WaitForSingleObject(hRemoteThread, INFINITE);
    CloseHandle(hRemoteThread);

    printf("[+] DLL injected successfully.\n");

    // Resume the main thread
    ResumeThread(pi.hThread);
    printf("[FINISHED] Process resumed.\n");

cleanup:
    // Cleanup
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    FreeLibrary(hKernel32);

    return 0;
}